// prja.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\..\classes\Udp.h"
SOCKET s;

bool CreateSocket(u_short port) {
	WSADATA wsaData; bool ret = false;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("Windows sockets 2.2 startup fail! process exit\r\n");
	}
	else {
		printf("Using %s (Status: %s)\n", wsaData.szDescription, wsaData.szSystemStatus);
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
				std::thread(thread_recv);
				std::thread(thread_send);
				printf("Server Start Successed on port %d\r\n", port);
				int tolen = sizeof(to);
				tolen = sendto(s, "test", 4, 0, (sockaddr*)&to, tolen);
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
	getchar();
    return 0;
}

