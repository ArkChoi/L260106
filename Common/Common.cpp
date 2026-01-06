#include "Common.h"

int RecvPacket(SOCKET Socket, char* Buffer)
{
	//Header를 다 받을때까지 기다림
	int PacketSize = 0;
	int RecvBytes = recv(Socket, (char*)&PacketSize, sizeof(PacketSize), MSG_WAITALL);
	if (RecvBytes <= 0)
	{
		return RecvBytes;
	}

	PacketSize = ntohl(PacketSize);

	//실제 패킷 사이즈만큼 기다림
	RecvBytes = recv(Socket, Buffer, PacketSize, MSG_WAITALL);

	return RecvBytes;
}

int SendPacket(SOCKET Socket, const flatbuffers::FlatBufferBuilder& Builder)
{
	int PacketSize = (int)Builder.GetSize();
	PacketSize = htonl(PacketSize);

	//Header Send
	int SentBytes = send(Socket, (char*)&PacketSize, sizeof(PacketSize), 0);
	if (SentBytes <= 0)
	{
		return PacketSize;
	}

	SentBytes = send(Socket, (char*)Builder.GetBufferPointer(), (int)Builder.GetSize(), 0);

	return PacketSize;
}