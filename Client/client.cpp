#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <process.h>
#include <iostream>
#include <conio.h>

#include "Common.h"

#pragma comment(lib, "Common.lib")

#pragma comment(lib, "ws2_32")

using namespace std;

string UserName;

unsigned RecvThread(void* Arg)
{
	SOCKET ServerSocket = *(SOCKET*)Arg;

	flatbuffers::FlatBufferBuilder RecvBuilder;

	char Buffer[1024] = { 0, };


	RecvPacket(ServerSocket, Buffer);

	auto UserEvent = UserEvents::GetEventData(Buffer);
	switch (UserEvent->data_type())
	{
	case UserEvents::EventType_S2C_Login:
		auto S2C_Login = UserEvent->data_as_S2C_Login();
		cout << S2C_Login->player_id() << endl;
		cout << (int)S2C_Login->color()->r() << endl;
		cout << S2C_Login->message()->c_str() << endl;
		break;
	}

	return 0;
}

unsigned SendThread(void* Arg)
{
	char Buffer[1024] = { 0, };

	cout << "Start ";
	_getch();

	SOCKET ServerSocket = *(SOCKET*)Arg;


	flatbuffers::FlatBufferBuilder SendBuilder;
	auto C2S_LoginData = UserEvents::CreateC2S_Login(SendBuilder,
		SendBuilder.CreateString("Choi"),
		SendBuilder.CreateString("q1234"));

	auto EventData = UserEvents::CreateEventData(SendBuilder, 0, UserEvents::EventType_C2S_Login, C2S_LoginData.Union());

	SendBuilder.Finish(EventData);

	int SentBytes = SendPacket(ServerSocket, SendBuilder);
	cout << "Send : " << SentBytes << endl;


	return 0;
}

int main()
{
	WSAData wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	char Buffer[1024] = { 0, };


	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = AF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("192.168.0.3");
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