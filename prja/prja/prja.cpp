// prja.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <stdint.h>
#include <list>
#include <map>
#include <algorithm>  
#include <functional>
#include <fcntl.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cmddef.h>
#pragma comment(lib, "ws2_32.lib")

SOCKET s;

int send_cmd_init(SOCKET s,const char* ip, const uint16_t port) {
	cmdbase cmd; int sended = 0; sockaddr_in to;
	cmd.cmd = init;
	cmd.d = GetTickCount64();
	to.sin_family = AF_INET;
	to.sin_port = htons(port);
	to.sin_addr.s_addr = inet_addr(ip);

	sended = sendto(s, (char*)&cmd, sizeof(cmd), 0, (sockaddr*)&to,sizeof(to) );
	return sended;
}


bool CreateSocket(u_short port) {
	WSADATA wsaData; bool ret = false;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("Windows sockets 2.2 startup fail! process exit\r\n");
	}
	else {
		printf("UDPP2P CLIENTS up! Using %s (Status: %s)\n", wsaData.szDescription, wsaData.szSystemStatus);
		printf("with API versions %d.%d to %d.%d\n\n",
			LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion),
			LOBYTE(wsaData.wHighVersion), HIBYTE(wsaData.wHighVersion));

		s = socket(AF_INET, SOCK_DGRAM, 0);
		if (s < 0) {
			printf("create socket error\r\n");
			ret = false;
		}
		else {
			sockaddr_in local;
			local.sin_family = AF_INET;
			local.sin_port = htons(port);
			local.sin_addr.s_addr = htonl(INADDR_ANY);
			sockaddr_in to;
			to.sin_family = AF_INET;
			to.sin_port = htons(3366);
			to.sin_addr.s_addr = inet_addr("127.0.0.1");
			int nResult = bind(s, (sockaddr*)&local, sizeof(sockaddr));
			DWORD err = GetLastError();
			if (nResult != SOCKET_ERROR) {
				//std::thread(thread_recv);
				//std::thread(thread_send);
				printf("Server Start Successed on port %d\r\n", port);
				ret = true;
				return ret;
			}
		}
	}

	return ret;
}


int main()
{
	CreateSocket(4789);
	while (true) {
		getchar();
		send_cmd_init(s, "127.0.0.1", 3366);
	}
    return 0;
}

