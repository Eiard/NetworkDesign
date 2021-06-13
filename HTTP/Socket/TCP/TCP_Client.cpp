
/*
	Socket客户端代码
	服务器127.0.0.1通信，端口8888
	建立连接后，发送给服务器，需要传输的文件路径
	若文件路径存在，接收服务器发送的文件流，发送完毕则关闭连接。
*/

#include <tchar.h>
#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>  
#include <string.h>  
#include <Ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

#define BUFFER_SIZE 2048
#define FILE_NAME_MAX_SIZE 512 

void TCP_Menu(void);

int main(int argc, char* argv[])
{
	WORD sockVersion;
	WSADATA wsaData;
	SOCKET client_socket;
	sockaddr_in server_addr;
	errno_t F_ERR;
	char* IP;
	int n;
	int port;
	FILE* fp;
	int length = 0;
	int Select;
	while (1) {
		TCP_Menu();
		printf("请输入选择:");
		scanf("%d", &Select);
		system("pause");
		system("cls");
		switch (Select) {
		case 1:
			/*
	初始化WSA，使得程序可以调用windows socket
	*/
			sockVersion = MAKEWORD(2, 2);
			wsaData;
			if (WSAStartup(sockVersion, &wsaData) != 0)
			{
				return 0;
			}

			/*
			创建监听用套接字，server_socket
			并检测是否创建成功
			*/
			client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); ;
			if (client_socket == INVALID_SOCKET) {
				//如果创建的socket无效，则结束程序
				perror("socket error !");
				break;
			}

			/*
			创建地址结构，server_addr，并设置端口和IP
			*/
			
			memset(&server_addr, 0, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			//要连接的服务器端口号 8888
			printf("请输入要连接的服务器端口号 port:");

			scanf("%d", &port);
			server_addr.sin_port = htons(port);
			//指定服务器的地址127.0.0.1

			printf("请输入服务器IP地址的数字个数 n:");
			scanf("%d", &n);
			IP = (char*)malloc(sizeof(char) * n + 4);
			printf("请输入服务器IP地址:");
			scanf("%s", IP);
			InetPton(AF_INET, IP, &server_addr.sin_addr.s_addr);


			//与地址server_addr建立连接
			if (connect(client_socket, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)))
			{
				perror("connect error !\n");
				return 0;
			}

			char REMOTE_file_name[FILE_NAME_MAX_SIZE + 1];
			memset(REMOTE_file_name, 0, FILE_NAME_MAX_SIZE + 1);
			printf("请输入要获取的服务器文件路径:\n");
			scanf_s("%s", REMOTE_file_name, FILE_NAME_MAX_SIZE);

			char LOCAL_file_name[FILE_NAME_MAX_SIZE + 1];
			memset(LOCAL_file_name, 0, FILE_NAME_MAX_SIZE + 1);
			printf("请输入保存文件的本地路径:\n");
			scanf_s("%s", LOCAL_file_name, FILE_NAME_MAX_SIZE);

			char buffer[BUFFER_SIZE];
			memset(buffer, 0, BUFFER_SIZE);
			strncpy_s(buffer, REMOTE_file_name, strlen(REMOTE_file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(REMOTE_file_name));

			// 向服务器发送buffer中的数据 
			if (send(client_socket, buffer, BUFFER_SIZE, 0) < 0)
			{
				perror("发送文件名失败:");
				exit(1);
			}

			// 打开文件，准备写入 

			F_ERR = fopen_s(&fp, LOCAL_file_name, "wb");
			if (F_ERR != 0)
			{
				printf("文件打开失败：%s\n", LOCAL_file_name);
				exit(1);
			}

			// 从服务器接收数据到buffer中 
			// 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止 
			memset(buffer, 0, BUFFER_SIZE);
			
			while ((length = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)
			{
				if (fwrite(buffer, sizeof(char), length, fp) < length)
				{
					printf("文件写入失败：%s\n", LOCAL_file_name);
					break;
				}
				memset(buffer, 0, BUFFER_SIZE);
			}

			printf("\n成功从服务器接收文件\n存入本地目录：%s\n", REMOTE_file_name, LOCAL_file_name);


			// 接收成功后，关闭文件，关闭socket、WSA 
			fclose(fp);
			closesocket(client_socket);
			system("pause");
			system("cls");
			break;
		case 2: {
			printf("退出成功!\n");
			system("pause");
			return 0;
		}
		default:
			printf("您输入有误请重新输入!\n");
			system("pause");
			system("cls");
			break;
		}
	}

	WSACleanup();
	system("pause");
	return 0;
}

void TCP_Menu(void) {
	printf("******************\n");
	printf("*  1.建立TCP连接 *\n");
	printf("*  2.退出        *\n");
}