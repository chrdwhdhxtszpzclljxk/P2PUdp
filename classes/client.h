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

class pkg {
public:
	void* pbuf;
	int32_t len;
	uint8_t id;
	time_t now;
};

struct h {
	uint8_t t : 1;
	uint8_t id : 7;
};

typedef std::list<pkg> pkglist;

class client {
public:
	client(sockaddr_in a);
	bool rsend(SOCKET s);

	bool send(void* b, int32_t l);

	bool ack(h*);

	virtual bool notify_recv(void* pbuf, int32_t len) { return false; };

private:
	pkglist dp,dd,dr;
	uint8_t id;
	sockaddr_in adr;
};
