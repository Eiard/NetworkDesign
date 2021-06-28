#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <tchar.h>
#include<direct.h>  //CreatThread ���������(windows)
#include<sys/types.h>  //fork����  ���߳�(Linuxϵͳ)
#include <winsock2.h>  //Socket
#include <Ws2tcpip.h>  //TCP/IP

#pragma comment(lib,"ws2_32.lib")	/* WinSockʹ�õĿ⺯�� */

char Web_Base[] = "D:\\Computer\\C++\\ComputerNetworkDesign\\Web_Server\\WebPage";  //����C���ļ�·����ʽ
typedef int Status;

#define OK 1
#define ERROR 0
#define BUFFER_SIZE 2048  //�����С
#define FILE_NAME_MAX_SIZE 512
#define ERROR_Msg_Size 512
#define Port 80

Status Init_Socket(SOCKET &server_socket, sockaddr_in &server_addr);  //��ʼ�������Socket IP �˿�
Status Server_Runing(SOCKET& server_socket);
Status Do_GetInfo(SOCKET& client_socket, char* fileName, char* buffer);
char* get_fileName(char* buf);  //��buffer���ҵ��ļ���
DWORD WINAPI HTTPServer(LPVOID lparam);

Status Init_Socket(SOCKET& server_socket, sockaddr_in& server_addr) {
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		//���������socket��Ч�����������
		perror("socket error !\n");
		return ERROR;
	}

	/*
	   ������ַ��server_addr�������ö˿ں�IP
	*/
	server_addr.sin_family = AF_INET;

	server_addr.sin_port = htons(Port);
	//INADDR_ANY��ʾ��������IP��ַ(�κ�IP����������)
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	//��socket���ַserver_addr��
	if (bind(server_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		perror("bind error !\n");
		return 0;
	}

	//server_socket��ʼ����
	if (listen(server_socket, 20) == SOCKET_ERROR) {
		perror("listen error !\n");
		return 0;
	}

	printf("���������ó�ʼ�����!\n");
	return OK;
}
Status Server_Runing(SOCKET& server_socket) {
	while (1){
		printf("�ȴ�����...\n");

		// ����ͻ��˵�socket��socket��ַ�ṹ
		SOCKET client_socket;
		sockaddr_in client_addr;

		int client_addr_length = sizeof(client_addr);
		// �����������󣬷���һ���µ�socket(������)�������socket����ͬ���ӵĿͻ���ͨ�� 
		// accept����������ӵ��Ŀͻ�����Ϣд��client_addr�� 
		client_socket = accept(server_socket, (SOCKADDR*)&client_addr, &client_addr_length);
		if (client_socket == INVALID_SOCKET){
			perror("Socket���ӽ���ʧ��:\n");
			continue;
		}

		char IP_BUFFER[256];
		memset(IP_BUFFER, 0, 256);
		InetNtop(AF_INET, &client_addr.sin_addr, IP_BUFFER, 256);
		printf("Socket���ӽ������ͻ���IPΪ��%s���˿�Ϊ��%d\n", IP_BUFFER, ntohs(client_addr.sin_port));

		//���տͻ�������ĵ��ļ�·��
		// recv�����������ݵ�������buffer�� 
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		if (recv(client_socket, buffer, BUFFER_SIZE, 0) < 0)
		{
			perror("�����ļ���ʧ��:\n");
			break;
		}
		printf("%s", buffer);

		// Ȼ���buffer(������)���ļ���������file_name�� 

		char* file_name = get_fileName(buffer);

		Do_GetInfo(client_socket, file_name,buffer);
		
		DWORD ThreadID;
		CreateThread(NULL, 0, HTTPServer, (LPVOID)client_socket, 0, &ThreadID);
		// �ر���ͻ��˵����� (�رտͻ���Socket)
		closesocket(client_socket);
	}
	return OK;
}
Status Do_GetInfo(SOCKET& client_socket, char* file_name,char* buffer){  //    I/O��
	// ���ļ�����ȡ�ļ�����
	//file_name���ļ���(����Ҫ���Ͼ��Ե�ַ��������ĵ�ַ)
	if (file_name == NULL) return ERROR;
	if (file_name[0]== '/'&&file_name[1]=='\0') {strcpy(file_name, "/index.html");}

	char file_addr[512] = {0};
	sprintf(file_addr, "%s%s", Web_Base, file_name);
	file_addr[sizeof(Web_Base)-1] = '\\';  //����Ŀ¼���/ ת��Ϊ"\\"
	FILE* fp;
	errno_t F_ERR = fopen_s(&fp, file_addr, "rb");  //����������ַ�ַ��� ת��Ϊ��Ӧ����λ�õ�ָ������

	if (F_ERR != 0){  //��ʧ��
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
		sprintf(Warning3, "%s%c%s%c%s%c%s%c%s%c%s%c%s", "<div align = center><img src = ", '"', "/Image/St.png", '"', " alt = ", '"',"������Ŷ!" ,'"'," align = ",'"',"middle",'"',"/ ></div>");
		send(client_socket, Warning3, strlen(Warning3), 0);

		return ERROR;
	}
	else{
		printf("��ʼ���ݴ��䣺%s\n", file_name);
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;
		// ÿ��ȡһ�����ݣ��㽫�䷢�͸��ͻ��ˣ�ѭ��ֱ���ļ�����Ϊֹ 
		while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0){
			if (send(client_socket, buffer, length, 0) < 0){
				printf("���ݷ���ʧ��:%s/n", file_name);
				break;
			}
			memset(buffer, 0, BUFFER_SIZE);
		}

		// �ر��ļ�
		fclose(fp);
		printf("�ļ��������:%s!\n", file_name);
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
	//���տͻ�������ĵ��ļ�·��
	// recv�����������ݵ�������buffer�� 
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	if (recv(client_socket, buffer, BUFFER_SIZE, 0) < 0)
	{
		perror("�����ļ���ʧ��:\n");
		return ERROR;
	}
	// Ȼ���buffer(������)���ļ���������file_name�� 

	char* file_name = get_fileName(buffer);

	Do_GetInfo(client_socket, file_name, buffer);

	// �ر���ͻ��˵����� (�رտͻ���Socket)
	closesocket(client_socket);

	return 0;
}
