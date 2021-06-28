#include"Base.h"




int main(void) {
	SOCKET server_socket;
	sockaddr_in server_addr;  //地址结构体

	/*
	   初始化WSA，使得程序可以调用windows socket
	*/
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0){
		return 0;
	}

	//初始化服务端Socket IP 端口 并且绑定bind
	Init_Socket(server_socket, server_addr);

	//传入服务器socket 运行服务器
	Server_Runing(server_socket);

	// 关闭监听用的socket本机Socket
	closesocket(server_socket);
	WSACleanup();
	return 0;
}
