//UDP server
//Kevin Lounsbury - 100654226
//Dimitrios Stefanakos - 100621470
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <time.h>

using std::string;
using std::cin;
using std::vector;
using std::thread;

#pragma comment(lib, "Ws2_32.lib")
//recvfrom(server_socket, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)& fromAddr, &fromlen) == SOCKET_ERROR
static struct sockaddr_in fromAddress;
static struct sockaddr_in toAddress;

static vector<string> IPAdresses;
static vector<bool> online;
static vector<struct sockaddr_in> fromAddrs;




//void ChatSend(SOCKET s)
//{
//	struct sockaddr_in fromAddr;
//	int fromlen;
//	fromlen = sizeof(fromAddr);
//	for (;;) {
//
//		const unsigned int BUF_LEN = 512;
//
//		char recv_buf[BUF_LEN];
//		int fromAdressLen = sizeof(fromAddress);
//
//		memset(recv_buf, 0, BUF_LEN);
//		if (recvfrom(s, recv_buf, BUF_LEN, 0, (struct sockaddr*) & fromAddr, &fromlen) == SOCKET_ERROR)
//		{
//			printf("recvfrom() failed...%d\n", WSAGetLastError());
//			exit(1);
//		}
//		
//		string msg = (string)recv_buf;
//
//		if (msg == "q\n")
//		{
//			fromAddress = struct sockaddr_in();
//			
//			toAddress = struct sockaddr_in();
//			break;
//		}
//
//		char* message = (char*)msg.c_str();
//
//		//send message to client
//		char ipbuf[INET_ADDRSTRLEN];
//		//inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf))
//		if (fromAddr.sin_port == fromAddress.sin_port)
//		{
//			if (sendto(s, message, BUF_LEN, 0, (struct sockaddr*) & toAddress, sizeof(toAddress)) == SOCKET_ERROR)
//			{
//				printf("sendto() failed %d\n", WSAGetLastError());
//				exit(1);
//			}
//		}
//		if (fromAddr.sin_port == toAddress.sin_port)
//		{
//			if (sendto(s, message, BUF_LEN, 0, (struct sockaddr*) & fromAddress, sizeof(fromAddress)) == SOCKET_ERROR)
//			{
//				printf("sendto() failed %d\n", WSAGetLastError());
//				exit(1);
//			}
//		}
//
//	}
//}

//void ChatService(SOCKET sock)
//{
//	ChatSend(sock);
//	//thread thread11(ChatReceive, sock);
//	//thread thread12(ChatSend, sock);
//	//thread11.join();
//	//thread12.join();
//}
void RecieveFrom(SOCKET s)
{
	const unsigned int BUF_LEN = 512;

	char recv_buf[BUF_LEN];



	// Struct that will hold the IP address of the client that sent the message (we don't have accept() anymore to learn the address)
	struct sockaddr_in fromAddr;
	int fromlen;
	fromlen = sizeof(fromAddr);

	
	bool newIP = true;
	for (;;) {
		memset(recv_buf, 0, BUF_LEN);
		if (recvfrom(s, recv_buf, 512, 0, (struct sockaddr*) & fromAddr, &fromlen) == SOCKET_ERROR)
		{
			//printf("recvfrom() failed...%d\n", WSAGetLastError());
			//exit(1);
		}
		
		char ipbuf[INET_ADDRSTRLEN];
		if ((string)recv_buf == "Online\n")
		{
			for (int i = 0; i < IPAdresses.size(); i++)
			{
				if (inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)) == IPAdresses[i])
					newIP = false;
			}
			if (newIP)
			{
				IPAdresses.push_back(inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)));
				fromAddrs.push_back(fromAddr);
				online.push_back(true);
			}
			newIP = true;
			printf("%s - %s", inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)), recv_buf);
		}
		else if ((string)recv_buf == "1")
		{
			for (int i = 0; i < fromAddrs.size(); i++)
			{
				if (inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)) == IPAdresses[i])
					online[i] = true;
			}
		}
		else if(fromAddr.sin_port != fromAddress.sin_port && fromAddr.sin_port != toAddress.sin_port){
			for (int i = 0; i < fromAddrs.size(); i++)
			{
				if((string)recv_buf == (IPAdresses[i]+"\n"))
				{
					fromAddress = fromAddr;
					toAddress = fromAddrs[i];
					//thread thr(ChatService, s);
				}
			}
			if ((string)recv_buf != "" && (string)recv_buf != "\n")
				printf("\nReceived: %s\n\n", recv_buf);

		}
		if (fromAddr.sin_port == fromAddress.sin_port)
		{
			string msg = (string)recv_buf;
			

			if (sendto(s, recv_buf, 512, 0, (struct sockaddr*) & toAddress, sizeof(toAddress)) == SOCKET_ERROR)
			{
				printf("sendto() failed %d\n", WSAGetLastError());
				exit(1);
			}
			if (msg == "q\n")
			{
				fromAddress = struct sockaddr_in();

				toAddress = struct sockaddr_in();
				
			}
		}
		if (fromAddr.sin_port == toAddress.sin_port)
		{
			string msg = (string)recv_buf;
			if (sendto(s, recv_buf, 512, 0, (struct sockaddr*) & fromAddress, sizeof(fromAddress)) == SOCKET_ERROR)
			{
				printf("sendto() failed %d\n", WSAGetLastError());
				exit(1);
			}
			if (msg == "q\n")
			{
				fromAddress = struct sockaddr_in();

				toAddress = struct sockaddr_in();
				
			}
		}
		//		printf("Dest IP address: %s\n", inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)));
		//		printf("Source IP address: %s\n", inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)));
		//for (int i = 0; i < fromAddrs.size(); i++)
		//{
		//	printf("%s\n", IPAdresses[i].c_str());
		//}
	}
	//thr.join();
}

void SendTo(SOCKET s)
{
	for (;;) {
		

		printf("Enter message: ");
		string line;
		getline(std::cin, line);
		line += "\n";
		char* message = (char*)line.c_str();

		//send message to server
		for (int i = 0; i < fromAddrs.size(); i++)
		{
			if (sendto(s, message, 512, 0, (struct sockaddr*) & fromAddrs[i], sizeof(fromAddrs[i])) == SOCKET_ERROR)
			{
				printf("sendto() failed %d\n", WSAGetLastError());
				exit(1);
			}
		}
	}
}

void SendStatus(SOCKET s)
{
	float appTime = 0.0;
	float oldTime = (float)clock()/CLOCKS_PER_SEC;
	float deltaTime;
	for (;;) {
		deltaTime = (float)clock() / CLOCKS_PER_SEC - oldTime;
		oldTime = (float)clock() / CLOCKS_PER_SEC;
		appTime += deltaTime;
		if (appTime > 5.0f)
		{
			string status = "\n";

			for (int i = 0; i < fromAddrs.size(); i++)
			{
				status += IPAdresses[i];
				status += " - ";

				if (fromAddrs[i].sin_port == fromAddress.sin_port || fromAddrs[i].sin_port == toAddress.sin_port)
					status += "Chatting\n";
				else if (online[i])
					status += "Online\n";
				else if(!online[i])
					status += "Offline\n";
			}

			char* message = (char*)status.c_str();

			//send message to server
			for (int i = 0; i < fromAddrs.size(); i++)
			{
				if (sendto(s, message, 512, 0, (struct sockaddr*) & fromAddrs[i], sizeof(fromAddrs[i])) == SOCKET_ERROR)
				{
					printf("sendto() failed %d\n", WSAGetLastError());
					exit(1);
				}

				//string ping = "1";
				//
				//if (sendto(s, (char*)ping.c_str(), 512, 0, (struct sockaddr*) & fromAddrs[i], sizeof(fromAddrs[i])) == SOCKET_ERROR)
				//{
				//	printf("sendto() failed %d\n", WSAGetLastError());
				//	exit(1);
				//}
				//

			}
			appTime = 0.0f;
		}
	}
}

void Ping(SOCKET s)
{
	float appTime = 0.0;
	float oldTime = (float)clock() / CLOCKS_PER_SEC;
	float deltaTime;
	for (;;) {
		deltaTime = (float)clock() / CLOCKS_PER_SEC - oldTime;
		oldTime = (float)clock() / CLOCKS_PER_SEC;
		appTime += deltaTime;
		if (appTime > 1.0f)
		{
			string ping = "1";
			//send message to server
			for (int i = 0; i < fromAddrs.size(); i++)
			{
				
				if (sendto(s, (char*)ping.c_str(), 512, 0, (struct sockaddr*) & fromAddrs[i], sizeof(fromAddrs[i])) == SOCKET_ERROR)
				{
					printf("sendto() failed %d\n", WSAGetLastError());
					exit(1);
				}
				
				online[i] = false;
			}
			appTime = 0.0f;
		}
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

	//Create a Server socket

	struct addrinfo* ptr = NULL, hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, "8888", &hints, &ptr) != 0) {
		printf("Getaddrinfo failed!! %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	SOCKET server_socket;

	server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (server_socket == INVALID_SOCKET) {
		printf("Failed creating a socket %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Bind socket

	if (bind(server_socket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
		printf("Bind failed: %d\n", WSAGetLastError());
		closesocket(server_socket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;
	}
	u_long iMode = 0;
	int iResult = ioctlsocket(server_socket, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{
		printf("ioctlsocket failed with error: %ld\n", iResult);
	}
	printf("Waiting for Data...\n\n");

	// Receive msg from client
	
	//
	//vector<string> IPAdresses;
	//vector<struct sockaddr_in> fromAddrs;
	//
	//bool newIP = true;
	//


	
	//for (;;) 
	{

		//memset(recv_buf, 0, BUF_LEN);
		//if (recvfrom(server_socket, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*) & fromAddr, &fromlen) == SOCKET_ERROR)
		////if(thread thread1(recvfrom, server_socket, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*) & fromAddr, &fromlen) ==SOCKET_ERROR)
		//{
		//	printf("recvfrom() failed...%d\n", WSAGetLastError());
		//	return 1;
		//}
		thread thread1(RecieveFrom, server_socket);
		//char ipbuf[INET_ADDRSTRLEN];
		//if ((string)recv_buf == "Online\n")
		//{
		//	for (int i = 0; i < IPAdresses.size(); i++)
		//	{
		//		if (inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)) == IPAdresses[i])
		//			newIP = false;
		//	}
		//	if (newIP)
		//	{
		//		IPAdresses.push_back(inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)));
		//	}
		//	newIP = true;
		//	fromAddrs.push_back(fromAddr);
		//
		//}
		//printf("\nReceived: %s\n\n", recv_buf);
		////		printf("Dest IP address: %s\n", inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)));
		////		printf("Source IP address: %s\n", inet_ntop(AF_INET, &fromAddr, ipbuf, sizeof(ipbuf)));
		//for (int i = 0; i < fromAddrs.size(); i++)
		//{
		//	printf("%s\n", IPAdresses[i].c_str());
		//}
		//

		thread thread2(SendTo, server_socket);
		thread thread3(SendStatus, server_socket);
		thread thread4(Ping, server_socket);

		//printf("Enter message: ");
		//string line;
		//getline(std::cin, line);
		//line += "\n";
		//char* message = (char*)line.c_str();
		//
		////send message to server
		//for (int i = 0; i < fromAddrs.size(); i++)
		//{
		//	if (sendto(server_socket, message, BUF_LEN, 0, (struct sockaddr*) & fromAddrs[i], sizeof(fromAddrs[i])) == SOCKET_ERROR)
		//	{
		//		printf("sendto() failed %d\n", WSAGetLastError());
		//		return 1;
		//	}
		//}
		//
		//
		thread1.join();
		thread2.join();
		thread3.join();
		thread4.join();
	}
	closesocket(server_socket);
	freeaddrinfo(ptr);
	WSACleanup();

	return 0;
}