
/*
Socket�ͻ��˴���
�������������Ϣ
������IP��127.0.0.1
�������˿ڣ�8887
�Ӽ����ж�ȡ��Ϣ�����͸������
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
	printf("*    ��ӭʹ�ÿͻ���            *\n");
	printf("*    1.�����ͻ����׽���        *\n");
	printf("*    2.�����˷�������        *\n");
	printf("*    3.�˳��ó���              *\n");
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
		printf("��ѡ��");
		scanf("%d", &Select);
		system("cls");
		switch (Select) {
		case 1: 
			/*
			��ʼ��WSA��ʹ�ó�����Ե���windows socket
			*/
			sockVersion = MAKEWORD(2, 2);
			
			if (WSAStartup(sockVersion, &wsaData) != 0)
			{
				return 0;
			}

			/*
			�����ͻ����׽��֣�client_socket
			������UDP
			������Ƿ񴴽��ɹ�
			*/
			client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (client_socket == INVALID_SOCKET) {
				//���������socket��Ч�����������
				perror("socket error !\n");
				return 0;
			}

			/*
			������ַ��server_addr�������ö˿ں�IP
			*/
			
			memset(&server_addr, 0, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			//Ҫ���ӵķ������˿ں� 8887
			printf("������Ҫ���ӵķ������˿ں� port:");
			
			scanf("%d", &port);
			server_addr.sin_port = htons(port);
			int n;
			printf("�����������IP��ַ�����ָ��� n:");
			scanf("%d", &n);
			IP = (char*)malloc(sizeof(char)*n+4);
			printf("�����������IP��ַ:");
			scanf("%s", IP);


			//�󶨷������ĵ�ַ127.0.0.1

			//��������IP���
			InetPton(AF_INET, IP, &server_addr.sin_addr.s_addr);
			system("pause");
			printf("��ʼ���ɹ�!");
			system("cls");
			break;
		
		case 2: 
			printf("�����뷢�͸�����������Ϣ��\n");
			printf("��ǰ�����ַ�����out���Զ��˳�����Ϣ���Ͷ�\n");
			printf("���ӷ������Ķ˿�Ϊ%d \n",port);
			getchar();
			while (1) {
				char buffer[BUFFER_SIZE];
				memset(buffer, 0, BUFFER_SIZE);

				// �Ӽ����ж�ȡ���뵽BUFFER��
				gets_s(buffer, BUFFER_SIZE);

				if (buffer[0] == 'o'&&buffer[1]=='u'&&buffer[2]=='t') {
					printf("�˳��ɹ�!");
					system("pause");
					system("cls");
					break;
				}

				//�������ݸ�������
				if (sendto(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(SOCKADDR)) == -1)
				{
					perror("������Ϣʧ��:\n");
					break;
				}

			}
			break;
		
		case 3: {
			printf("�˳��ɹ�!\n");
			// �رշ�����socket ��ע�� WSA
			closesocket(client_socket);
			WSACleanup();
			system("pause");
			return 0;
		}
		default:
			printf("��������������������!\n");
			system("pause");
			system("cls");
			break;
	}
}

}