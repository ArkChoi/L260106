#pragma once

#define NOMINMAX

#include <WinSock2.h>
#include <Windows.h>

#include "flatbuffers/flatbuffers.h"
#include "UserEvents_generated.h"

#pragma comment(lib,"ws2_32")

int RecvPacket(SOCKET Socket, char* Buffer);

int SendPacket(SOCKET Socket, const flatbuffers::FlatBufferBuilder& Builder);
