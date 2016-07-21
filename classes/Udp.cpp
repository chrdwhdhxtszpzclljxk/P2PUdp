#include "Udp.h"

#pragma comment(lib, "ws2_32.lib")

uint16_t CUdp::port;

bool client::ok(uint8_t id) {
	std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
	if (data.front().id == id) data.pop_front();
	return true;
}

bool client::rsend(SOCKET s) {
	std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
	if (!data.empty()) {
		pkg p = data.front();
		if (nowid == p.id) return false;
		nowid = p.id;
		::sendto(s, (char*)p.pbuf, p.len, 0, (sockaddr*)&adr, sizeof(adr));
	}
	return true;
}

client::client(void* b, int32_t l, sockaddr_in a) :adr(a), id(0) {
	std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
	nowid = -1;
	pkg p;
	p.pbuf = b;
	p.len = l+1;
	p.id = id++;
	((char*)b)[l] = p.id;
	data.push_back(p);
};

bool client::send(void* b, int32_t l) {
	std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
	if (data.size() >= 200) return false;
	
	pkg p;
	p.pbuf = b;
	p.len = l+1;
	p.id = id++;
	((char*)b)[l] = p.id;
	data.push_back(p);
	return true;
}

void CUdp::thread1() {
	run = true;
	int32_t len = 1024 * 8;
	int32_t recvd = 0;
	sockaddr_in peer;
	int32_t adrlen = sizeof(peer);
	char* pbuf = new char[len];
	while (run) {
		recvd = recvfrom(s, pbuf, len, 0, (sockaddr*)&peer,&adrlen);
		if (recvd == -1) {
			if (GetLastError() != WSAEWOULDBLOCK) { break; }
			else {
				std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
				peerlist::iterator iter;
				for (iter = this->peer.begin(); iter != this->peer.end(); iter++) {
					(*iter).second->rsend(s);
				}
				continue;
			}
		}
		std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
		peerlist::iterator iter = this->peer.find(peer);
		if (iter != this->peer.end()) {
			client* r = this->peer.find(peer)->second;
			uint8_t id = pbuf[len - 1];
			r->ok(id);
			r->notify_recv(pbuf, len - 1);
			r->rsend(s);
		}
		else {
			this->peer[peer] = new client();
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
		peer[*adr] = new client(pbuf, len, *adr);
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
				// Set the socket I/O mode: In this case FIONBIO
				// enables or disables the blocking mode for the
				// socket based on the numerical value of iMode.
				// If iMode = 0, blocking is enabled;
				// If iMode != 0, non-blocking mode is enabled.
				u_long iMode = 1;  //non-blocking mode is enabled.
				ioctlsocket(s, FIONBIO, &iMode);
				thr = std::thread(&CUdp::thread1,this);
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
