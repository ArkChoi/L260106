#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include <winsock2.h>
#include <Windows.h>
#include <process.h>
#include <iostream>


#pragma comment(lib, "ws2_32")

using namespace std;

string UserName;


unsigned RecvThread(void* Arg)
{
	SOCKET ServerSocket = *(SOCKET*)Arg;

	char Buffer[1024] = { 0, };

	while (true)
	{
		int PacketSize = 0;
		int RecvBytes = recv(ServerSocket, (char*)&PacketSize, sizeof(PacketSize), MSG_WAITALL);

		if (RecvBytes <= 0)
		{
			closesocket(ServerSocket);
			break;
		}

		PacketSize = ntohl(PacketSize);

		//실제 패킷 사이즈만큼 기다림
		char Buffer[4096] = { 0, };
		RecvBytes = recv(ServerSocket, Buffer, PacketSize, MSG_WAITALL);
		if (RecvBytes <= 0)
		{
			closesocket(ServerSocket);
			break;
		}

		cout << Buffer << endl;
	}
	return 0;
}

unsigned SendThread(void* Arg)
{
	SOCKET ServerSocket = *(SOCKET*)Arg;


	while (true)
	{
		char Buffer[1024] = { 0, };

		cout << "Chat : ";
		cin.getline(Buffer, sizeof(Buffer));


		int PacketSize = 0;
		PacketSize = htonl(PacketSize);

		int SendBytes = send(ServerSocket, (char*)&PacketSize, sizeof(PacketSize), 0);

		//SendBytes = send(ServerSocket, JSONString.c_str(), Data.Length(), 0);
	}
	return 0;
}




int main()
{
	WSAData wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	char Buffer[1024] = { 0, };

	cout << "Name : ";
	cin.getline(Buffer, 1024);
	UserName = Buffer;



	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = AF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("218.156.17.164");
	ServerSockAddr.sin_port = htons(30000);

	int Result = connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	HANDLE SocketThread[2];
	SocketThread[0] = (HANDLE)_beginthreadex(0, 0, RecvThread, &ServerSocket, 0, 0);
	SocketThread[1] = (HANDLE)_beginthreadex(0, 0, SendThread, &ServerSocket, 0, 0);

	DWORD HResult = WaitForMultipleObjects(2, SocketThread, true, INFINITE);

	CloseHandle(SocketThread[0]);
	CloseHandle(SocketThread[1]);

	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}
