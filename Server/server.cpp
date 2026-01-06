#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#define FD_SETSIZE			100

#include <iostream>

#include "Common.h"
//#include "ChatPacket.h"

#pragma comment(lib, "ws2_32")



using namespace std;

string PrintAddress(SOCKET InSocket)
{
	SOCKADDR_IN GetSocketAddr;
	memset(&GetSocketAddr, 0, sizeof(GetSocketAddr));
	int GetSocketAddrLength = sizeof(GetSocketAddr);

	getpeername(InSocket, (SOCKADDR*)&GetSocketAddr, &GetSocketAddrLength);

	char Buffer[1024];
	sprintf(Buffer, "%s:%d", inet_ntoa(GetSocketAddr.sin_addr), ntohs(GetSocketAddr.sin_port));

	return Buffer;
}

int main()
{
	WSAData WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ListenSockAddr;
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));
	ListenSockAddr.sin_family = AF_INET;
	ListenSockAddr.sin_addr.s_addr = inet_addr("192.168.0.100");
	ListenSockAddr.sin_port = htons(30000);

	bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	listen(ListenSocket, 5);

	fd_set ReadSocketList;
	FD_ZERO(&ReadSocketList);
	FD_SET(ListenSocket, &ReadSocketList);

	cout << "Start Server" << endl;

	//[1][1][1]  [1][1]   ->   [1][1]
	TIMEVAL Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 100;
	while (true)
	{
		fd_set CopyReadSocketList = ReadSocketList;

		int ChangeCount = select(0, &CopyReadSocketList, nullptr, nullptr, &Timeout);
		if (ChangeCount == 0)
		{
			//cout << "Wait" << endl;
			//입력 없을때 서버 작업
			continue;
		}

		//실제 연결처리 및 패킷 처리
		for (int i = 0; i < (int)ReadSocketList.fd_count; ++i)
		{
			SOCKET SelectSocket = ReadSocketList.fd_array[i];
			if (FD_ISSET(SelectSocket, &CopyReadSocketList))
			{
				if (SelectSocket == ListenSocket)
				{
					//연결 요청
					SOCKADDR_IN ClientSockAddr;
					memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
					int ClientSockAddrLength = sizeof(ClientSockAddr);
					SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &ClientSockAddrLength);
					//나중에 세션 만든다고..
					FD_SET(ClientSocket, &ReadSocketList);
					cout << "Client connect : " << PrintAddress(ClientSocket) << endl;
				}
				else
				{
					char Buffer[4096] = { 0, };
					int RecvBytes = RecvPacket(SelectSocket, Buffer);

					//flatbuffer deserialize 하자 | 목표
				}
			}
		}
	}




	closesocket(ListenSocket);


	WSACleanup();


	return 0;

}