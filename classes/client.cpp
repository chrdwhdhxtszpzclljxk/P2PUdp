#include "client.h"
#include "Udp.h"


bool client::ack(h* id) {
	std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
	if (dd.front().id == id->id) dd.pop_front();
	return true;
}

bool client::rsend(SOCKET s) {
	std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
	if (!dp.empty() && dd.empty()) {
		pkg p = dp.front();
		dp.pop_front();
		::sendto(s, (char*)p.pbuf, p.len, 0, (sockaddr*)&adr, sizeof(adr));
		p.now = time(NULL);
		dd.push_back(p);
	}
	return true;
}

client::client(sockaddr_in a) :adr(a), id(0) {
};

bool client::send(void* b, int32_t l) {
	std::lock_guard<std::recursive_mutex> guard1(CUdp::me()->mut);
	if (dp.size() >= 120) return false; // 7bit Ϊ127

	pkg p;
	p.pbuf = b;
	p.len = l + 1;
	p.id = id++;
	h h1;
	h1.t = 0;
	h1.id = p.id;
	((char*)b)[l] = p.id;
	dp.push_back(p);
	return true;
}