#pragma once
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

class basecmd {
public:
	uint8_t id;
};



//map的比较函数
struct cmp_key
{
	bool operator()(const sockaddr_in &k1, const sockaddr_in &k2)const
	{
		if (k1.sin_port < k2.sin_port && k1.sin_addr.S_un.S_addr < k2.sin_addr.S_un.S_addr) return true;
		return false;
	}
};

typedef std::map<sockaddr_in, client*, cmp_key> peerlist;

class CUdp
{
public:
	static void setup(uint16_t p) {
		port = p;
	}
	static CUdp* me() {
		static CUdp* me = nullptr;
		if (me == nullptr) {
			me = new CUdp();
			if (!me->init()) {
				delete me;
				me = nullptr;
			}
		}
		return me;
	}

	bool sendto(void*,int32_t, sockaddr_in*);
	char* alloc(int32_t len);
	std::recursive_mutex mut;
private:
	void thread_recv();
	void thread_send();
	virtual bool init();
	CUdp();
	~CUdp();
	SOCKET s;
	bool run;
	peerlist peer;
	static uint16_t port;
	std::thread thr_recv,thr_send;
};

