#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <tchar.h>
#include<direct.h>  //CreatThread 创建多进程(windows)
#include<sys/types.h>  //fork函数  多线程(Linux系统)
#include <winsock2.h>  //Socket
#include <Ws2tcpip.h>  //TCP/IP

#pragma comment(lib,"ws2_32.lib")	/* WinSock使用的库函数 */

char Web_Base[] = "D:\\Computer\\C++\\ComputerNetworkDesign\\Web_Server\\WebPage";  //用于C的文件路径格式
typedef int Status;

#define OK 1
#define ERROR 0
#define BUFFER_SIZE 2048  //缓存大小
#define FILE_NAME_MAX_SIZE 512
#define ERROR_Msg_Size 512
#define Port 80

Status Init_Socket(SOCKET &server_socket, sockaddr_in &server_addr);  //初始化服务端Socket IP 端口
Status Server_Runing(SOCKET& server_socket);
Status Do_GetInfo(SOCKET& client_socket, char* fileName, char* buffer);
char* get_fileName(char* buf);  //从buffer中找到文件名
DWORD WINAPI HTTPServer(LPVOID lparam);

Status Init_Socket(SOCKET& server_socket, sockaddr_in& server_addr) {
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		//如果创建的socket无效，则结束程序
		perror("socket error !\n");
		return ERROR;
	}

	/*
	   创建地址，server_addr，并设置端口和IP
	*/
	server_addr.sin_family = AF_INET;

	server_addr.sin_port = htons(Port);
	//INADDR_ANY表示本机任意IP地址(任何IP都可以连接)
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	//将socket与地址server_addr绑定
	if (bind(server_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		perror("bind error !\n");
		return 0;
	}

	//server_socket开始监听
	if (listen(server_socket, 20) == SOCKET_ERROR) {
		perror("listen error !\n");
		return 0;
	}

	printf("服务器配置初始化完成!\n");
	return OK;
}
Status Server_Runing(SOCKET& server_socket) {
	while (1){
		printf("等待连接...\n");

		// 定义客户端的socket和socket地址结构
		SOCKET client_socket;
		sockaddr_in client_addr;

		int client_addr_length = sizeof(client_addr);
		// 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信 
		// accept函数会把连接到的客户端信息写到client_addr中 
		client_socket = accept(server_socket, (SOCKADDR*)&client_addr, &client_addr_length);
		if (client_socket == INVALID_SOCKET){
			perror("Socket连接建立失败:\n");
			continue;
		}

		char IP_BUFFER[256];
		memset(IP_BUFFER, 0, 256);
		InetNtop(AF_INET, &client_addr.sin_addr, IP_BUFFER, 256);
		printf("Socket连接建立，客户端IP为：%s，端口为：%d\n", IP_BUFFER, ntohs(client_addr.sin_port));

		//接收客户端请求的的文件路径
		// recv函数接收数据到缓冲区buffer中 
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		if (recv(client_socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			perror("接收文件名失败:\n");
			break;
		}
		printf("%s", buffer);

		// 然后从buffer(缓冲区)的文件名拷贝到file_name中 

		char* file_name = get_fileName(buffer);

		Do_GetInfo(client_socket, file_name,buffer);
		
		DWORD ThreadID;
		CreateThread(NULL, 0, HTTPServer, (LPVOID)client_socket, 0, &ThreadID);
		// 关闭与客户端的连接 (关闭客户端Socket)
		closesocket(client_socket);
	}
	return OK;
}
Status Do_GetInfo(SOCKET& client_socket, char* file_name,char* buffer){  //    I/O流
	// 打开文件并读取文件数据
	//file_name是文件名(还需要加上绝对地址组成完整的地址)
	if (file_name == NULL) return ERROR;
	if (file_name[0]== '/'&&file_name[1]=='\0') {strcpy(file_name, "/index.html");}

	char file_addr[512] = {0};
	sprintf(file_addr, "%s%s", Web_Base, file_name);
	file_addr[sizeof(Web_Base)-1] = '\\';  //将根目录后的/ 转化为"\\"
	FILE* fp;
	errno_t F_ERR = fopen_s(&fp, file_addr, "rb");  //将服务器地址字符串 转化为对应绝对位置的指针坐标

	if (F_ERR != 0){  //打开失败
		printf("\n%s is not found\n", file_addr);

		char Warning1[ERROR_Msg_Size] = { "<h1  align=0center0  style=0color:#FF45000>404 Not Found</h1>" };
		Warning1[11] = '"';
		Warning1[18] = '"';
		Warning1[27] = '"';
		Warning1[41] = '"';
		send(client_socket, Warning1, strlen(Warning1), 0);
		char Warning2[ERROR_Msg_Size] = { "<h2  align=0center0  style=0color:#97FFFF0>Please enter the correct URL</h2>" };
		Warning2[11] = '"';
		Warning2[18] = '"';
		Warning2[27] = '"';
		Warning2[41] = '"';
		send(client_socket, Warning2, strlen(Warning2), 0);
		char Warning3[ERROR_Msg_Size];
		sprintf(Warning3, "%s%c%s%c%s%c%s%c%s%c%s%c%s", "<div align = center><img src = ", '"', "/Image/St.png", '"', " alt = ", '"',"出错了哦!" ,'"'," align = ",'"',"middle",'"',"/ ></div>");
		send(client_socket, Warning3, strlen(Warning3), 0);

		return ERROR;
	}
	else{
		printf("开始数据传输：%s\n", file_name);
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;
		// 每读取一段数据，便将其发送给客户端，循环直到文件读完为止 
		while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0){
			if (send(client_socket, buffer, length, 0) < 0){
				printf("数据发送失败:%s/n", file_name);
				break;
			}
			memset(buffer, 0, BUFFER_SIZE);
		}

		// 关闭文件
		fclose(fp);
		printf("文件传输完成:%s!\n", file_name);
	}

	return OK;
}
char* get_fileName(char* buf){
	char* p = (char*)malloc(128);
	p = strchr(buf, ' ');
	if (p != NULL)
	{
		*p++ = '\0';
		if (strcmp(buf, "GET") == 0)
		{
			char* q = strchr(p, ' ');
			if (q != NULL)
			{
				*q = '\0';
				return p;
			}
		}
		return NULL;
	}
	return NULL;
}
DWORD WINAPI HTTPServer(LPVOID lparam) {
	SOCKET client_socket = (SOCKET)(LPVOID)lparam;
	//接收客户端请求的的文件路径
	// recv函数接收数据到缓冲区buffer中 
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	if (recv(client_socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		perror("接收文件名失败:\n");
		return ERROR;
	}
	// 然后从buffer(缓冲区)的文件名拷贝到file_name中 

	char* file_name = get_fileName(buffer);

	Do_GetInfo(client_socket, file_name, buffer);

	// 关闭与客户端的连接 (关闭客户端Socket)
	closesocket(client_socket);

	return 0;
}
