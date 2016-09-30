#include "Udp.h"

#pragma comment(lib, "ws2_32.lib")

uint16_t CUdp::port;


void CUdp::thread_send() {
	run = true;
	while (run) {
		std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
		peerlist::iterator iter;
		for (iter = this->peer.begin(); iter != this->peer.end(); iter++) {
			(*iter).second->rsend(s);
		}
	}
}

void CUdp::thread_recv() {
	run = true;
	int32_t len = 1024 * 8;
	int32_t recvd = 0;
	sockaddr_in peer;
	int32_t adrlen = sizeof(peer);
	char* pbuf = new char[len];
	while (run) {
		recvd = recvfrom(s, pbuf, len, 0, (sockaddr*)&peer,&adrlen);
		if (recvd == -1) {
			continue;
		}
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
	}
}

bool CUdp::sendto(void* pbuf, int32_t len, sockaddr_in* adr) {
	bool ret = false;
	std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
	peerlist::iterator it = peer.find(*adr);
	if (it != peer.end()) { 
		ret = (*it).second->send(pbuf, len);
	}
	else {
		client* p = new client(*adr);
		p->send(pbuf, len);
		peer[*adr] = p;
		
		ret = true;
	}
	return ret;
}


char* CUdp::alloc(int32_t len) {
	int32_t l = len + 1;
	char* ret = new char[l];
	memset(ret, 0, l);
	return ret;
}

bool CUdp::init() {
	WSADATA wsaData; bool ret = false;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		printf("Windows sockets 2.2 startup fail! process exit");
	}else {
		printf("Using %s (Status: %s)\n",wsaData.szDescription, wsaData.szSystemStatus);
		printf("with API versions %d.%d to %d.%d\n\n",
			LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion),
			LOBYTE(wsaData.wHighVersion), HIBYTE(wsaData.wHighVersion));

		s = socket(AF_INET, SOCK_DGRAM, 0);
		if (s < 0){
			printf("create socket error");
			ret = false;
		}else {
			sockaddr_in local;
			local.sin_family = AF_INET;
			local.sin_port = htons(port);
			local.sin_addr.s_addr = htonl(INADDR_ANY);
			int nResult = bind(s, (sockaddr*)&local, sizeof(sockaddr));
			DWORD err = GetLastError();
			if (nResult != SOCKET_ERROR) {
				thr_recv = std::thread(&CUdp::thread_recv,this);
				thr_send = std::thread(&CUdp::thread_send, this);
				ret = true;
			}
		}
	}

	return ret;
}

CUdp::CUdp()
{
}


CUdp::~CUdp()
{
}
