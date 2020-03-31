//UDP Client
//Kevin Lounsbury - 100654226
//Dimitrios Stefanakos 	- 100621470
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <thread>
#include <stdlib.h>

using std::string;
using std::thread;

#pragma comment(lib, "Ws2_32.lib")

void RecieveFrom(SOCKET cli_socket, sockaddr* ptr, size_t addrlen)
{
	char recv_buf[512];
	struct sockaddr_in fromAddr;
	int fromlen;
	fromlen = sizeof(fromAddr);
	for (;;) {


		memset(recv_buf, 0, 512);
		if (recvfrom(cli_socket, recv_buf, 512, 0, (struct sockaddr*) & fromAddr, &fromlen) == SOCKET_ERROR) {
			//printf("recvfrom() failed...%d\n", WSAGetLastError());
			//exit(1);
		}
		else if ((string)recv_buf == "1")
		{
			if (sendto(cli_socket, recv_buf, 512, 0, ptr, addrlen) == SOCKET_ERROR)
			{
				printf("sendto() failed %d\n", WSAGetLastError());
				exit(1);
			}
		}
		else
		{
			printf("\nReceived: %s\n", recv_buf);

		}


		//char ipbuf[INET_ADDRSTRLEN];
	}
}
void SendTo(SOCKET cli_socket, sockaddr* ptr, size_t addrlen)
{
	const unsigned int BUF_LEN = 512;

	char send_buf[BUF_LEN];
	memset(send_buf, 0, BUF_LEN);

	// Struct that will hold the IP address of the client that sent the message (we don't have accept() anymore to learn the address)
	struct sockaddr_in fromAddr;
	int fromlen;
	fromlen = sizeof(fromAddr);

	std::string connection = "Online\n";
	char* conecMessage = (char*)connection.c_str();
	if (sendto(cli_socket, conecMessage, BUF_LEN, 0,
		ptr, addrlen) == SOCKET_ERROR)
	{
		printf("sendto() failed %d\n", WSAGetLastError());
		exit(1);
	}
	for (;;) {


		//printf("Enter Message: ");
		std::string line;
		std::getline(std::cin, line);
		line += "\n";
		char* message = (char*)line.c_str();

		//send message to server
		if (sendto(cli_socket, message, BUF_LEN, 0,
			ptr, addrlen) == SOCKET_ERROR)
		{
			printf("sendto() failed %d\n", WSAGetLastError());
			exit(1);
		}
		//printf("\nYour message has been sent\n");
	}
	
}

int main() {

	//Initialize winsock
	WSADATA wsa;

	int error;
	error = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (error != 0) {
		printf("Failed to initialize %d\n", error);
		return 1;
	}

	//Create a client socket

	struct addrinfo* ptr = NULL, hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	string ipad;
	std::cout << "Enter IP: ";
	std::cin >> ipad;
	std::cout << "Enter IP to chat with other client.\n";
	std::cout << "Or just type to messagge the server.\n";
	std::cout << "When in a chat send 'q' to exit chat.\n";

	if (getaddrinfo(ipad.c_str(), "8888", &hints, &ptr) != 0) {
		printf("Getaddrinfo failed!! %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	SOCKET cli_socket;

	cli_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (cli_socket == INVALID_SOCKET) {
		printf("Failed creating a socket %d\n\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}



	thread thread1(SendTo, cli_socket, ptr->ai_addr, ptr->ai_addrlen);
	thread thread2(RecieveFrom, cli_socket, ptr->ai_addr, ptr->ai_addrlen);
	thread1.join();
	thread2.join();
	//const unsigned int BUF_LEN = 512;
	//
	//char send_buf[BUF_LEN];
	//memset(send_buf, 0, BUF_LEN);
	//
	//// Struct that will hold the IP address of the client that sent the message (we don't have accept() anymore to learn the address)
	//struct sockaddr_in fromAddr;
	//int fromlen;
	//fromlen = sizeof(fromAddr);
	//
	//char recv_buf[BUF_LEN];
	//
	//std::string connection = "Online\n";
	//char* conecMessage = (char*)connection.c_str();
	//if (sendto(cli_socket, conecMessage, BUF_LEN, 0,
	//	ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR)
	//{
	//	printf("sendto() failed %d\n", WSAGetLastError());
	//	return 1;
	//}
	//for (;;) {
	//
	//
	//	memset(recv_buf, 0, BUF_LEN);
	//	if (recvfrom(cli_socket, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*) & fromAddr, &fromlen) == SOCKET_ERROR) {
	//		printf("recvfrom() failed...%d\n", WSAGetLastError());
	//		return 1;
	//	}
	//
	//	printf("\nReceived: %s\n", recv_buf);
	//
	//	char ipbuf[INET_ADDRSTRLEN];
	//	//		printf("Dest IP address: %s\n", inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)));
	//	//		printf("Source IP address: %s\n", inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)));
	//
	//	printf("Enter Message: ");
	//	std::string line;
	//	std::getline(std::cin, line);
	//	line += "\n";
	//	char* message = (char*)line.c_str();
	//
	//	//send message to server
	//	if (sendto(cli_socket, message, BUF_LEN, 0,
	//		ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR)
	//	{
	//		printf("sendto() failed %d\n", WSAGetLastError());
	//		return 1;
	//	}
	//	printf("\nYour message has been sent\n");
	//
	//
	//}


	//Shutdown the socket

	if (shutdown(cli_socket, SD_BOTH) == SOCKET_ERROR) {
		printf("Shutdown failed!  %d\n", WSAGetLastError());
		closesocket(cli_socket);
		WSACleanup();
		return 1;
	}

	closesocket(cli_socket);
	freeaddrinfo(ptr);
	WSACleanup();

	return 0;
}