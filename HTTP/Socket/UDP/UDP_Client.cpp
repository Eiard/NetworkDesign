
/*
Socket客户端代码
向服务器发送消息
服务器IP：127.0.0.1
服务器端口：8887
从键盘中读取消息，发送给服务端
*/

#include <tchar.h>
#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>   
#include <string.h>   
#include <Ws2tcpip.h>


#pragma comment(lib,"ws2_32.lib")

#define BUFFER_SIZE 2048

void Client_Menu(void) {
	printf("********************************\n");
	printf("*    欢迎使用客户端            *\n");
	printf("*    1.创建客户端套接字        *\n");
	printf("*    2.向服务端发送数据        *\n");
	printf("*    3.退出该程序              *\n");
}


int main(int argc, char* argv[])
{
	WORD sockVersion;
	WSADATA wsaData;
	SOCKET client_socket;
	sockaddr_in server_addr;
	int Select;
	char* IP;
	int port = 0;
	while (TRUE) {
		Client_Menu();
		printf("请选择：");
		scanf("%d", &Select);
		system("cls");
		switch (Select) {
		case 1: 
			/*
			初始化WSA，使得程序可以调用windows socket
			*/
			sockVersion = MAKEWORD(2, 2);
			
			if (WSAStartup(sockVersion, &wsaData) != 0)
			{
				return 0;
			}

			/*
			创建客户端套接字，client_socket
			类型是UDP
			并检测是否创建成功
			*/
			client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (client_socket == INVALID_SOCKET) {
				//如果创建的socket无效，则结束程序
				perror("socket error !\n");
				return 0;
			}

			/*
			创建地址，server_addr，并设置端口和IP
			*/
			
			memset(&server_addr, 0, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			//要连接的服务器端口号 8887
			printf("请输入要连接的服务器端口号 port:");
			
			scanf("%d", &port);
			server_addr.sin_port = htons(port);
			int n;
			printf("请输入服务器IP地址的数字个数 n:");
			scanf("%d", &n);
			IP = (char*)malloc(sizeof(char)*n+4);
			printf("请输入服务器IP地址:");
			scanf("%s", IP);


			//绑定服务器的地址127.0.0.1

			//跟服务器IP相关
			InetPton(AF_INET, IP, &server_addr.sin_addr.s_addr);
			system("pause");
			printf("初始化成功!");
			system("cls");
			break;
		
		case 2: 
			printf("请输入发送给服务器的消息：\n");
			printf("第前三个字符输入out则自动退出该消息发送端\n");
			printf("连接服务器的端口为%d \n",port);
			getchar();
			while (1) {
				char buffer[BUFFER_SIZE];
				memset(buffer, 0, BUFFER_SIZE);

				// 从键盘中读取输入到BUFFER中
				gets_s(buffer, BUFFER_SIZE);

				if (buffer[0] == 'o'&&buffer[1]=='u'&&buffer[2]=='t') {
					printf("退出成功!");
					system("pause");
					system("cls");
					break;
				}

				//发送数据给服务器
				if (sendto(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(SOCKADDR)) == -1)
				{
					perror("发送消息失败:\n");
					break;
				}

			}
			break;
		
		case 3: {
			printf("退出成功!\n");
			// 关闭服务器socket 并注销 WSA
			closesocket(client_socket);
			WSACleanup();
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

}