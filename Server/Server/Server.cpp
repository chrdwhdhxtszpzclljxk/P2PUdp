// Server.cpp : 定义控制台应用程序的入口点。
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
#include "client.h"

#pragma comment(lib, "ws2_32.lib")

#define makekey(a,b,c) {\
c = a; \
c = c << 32; \
c = c | b;\
}

typedef std::map<uint64_t, client*> CLIENTS;
typedef std::lock_guard<std::recursive_mutex> _lock;


SOCKET s;
CLIENTS cs;
std::recursive_mutex mt;

std::atomic_bool run = false;
void thread_send();
void thread_recv();
bool CreateSocket(u_short port);


int main()
{
	printf("Server prepare to Start...\r\n");
	CreateSocket(3366);
	while (true) {
		getchar();
	}
    return 0;
}


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
			int nResult = bind(s, (sockaddr*)&local, sizeof(sockaddr));
			DWORD err = GetLastError();
			if (nResult != SOCKET_ERROR) {
				std::thread a(thread_recv);
				std::thread(thread_send);
				printf("Server Start Successed on port %d\r\n",port);
				ret = true;
				return ret;
			}
		}
	}

	return ret;
}



void thread_send() {
	run = true;
	while (run) {
		/*
		std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
		peerlist::iterator iter;
		for (iter = this->peer.begin(); iter != this->peer.end(); iter++) {
			(*iter).second->rsend(s);
		}
		*/
	}
}

void thread_recv() {
	run = true;
	uint32_t ip = 0;
	uint32_t port = 0;
	uint64_t id = 0;
	int32_t len = 1024 * 8;
	int32_t recvd = 0;
	sockaddr_in peer;
	int32_t adrlen = sizeof(peer);
	char* pbuf = new char[len];
	CLIENTS::iterator iter;
	client* client0;
	while (run) {
		recvd = recvfrom(s, pbuf, len, 0, (sockaddr*)&peer, &adrlen);
		if (recvd == -1) {
			continue;
		}
		ip = peer.sin_addr.S_un.S_addr;
		port = peer.sin_port;
		makekey(ip, port, id);
		_lock l(mt);
		iter = cs.find(id);
		client0 = nullptr;
		if (iter == cs.end()) {
			client0 = new client();
			cs[id] = client0;
			iter = cs.find(id);
		}
		else {
			client0 = iter->second;
		}

		if (client0 != nullptr) {
			client0->mi.insert(client0->mi.end(), pbuf, pbuf + recvd);
		}


		/*
		h* ph = (h*)(pbuf + recvd - 1);
		if (ph->t = 1) {
			std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
			peerlist::iterator iter = this->peer.find(peer);
			if (iter != this->peer.end()) {
				client* r = this->peer.find(peer)->second;
				r->ack(ph);
			}
		}
		else {
			client* r = new client(peer);
			r->notify_recv(pbuf, recvd - 1);
			this->peer[peer] = r;
		}
		*/
	}
}
