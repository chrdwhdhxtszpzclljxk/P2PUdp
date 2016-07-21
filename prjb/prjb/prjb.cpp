// prjb.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\..\classes\Udp.h"
#pragma warning(disable:4996)

int main()
{
	sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(1233);
	local.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	CUdp::setup(1234);
	char* msg = CUdp::me()->alloc(4);
	strncpy(msg, "test",4);
	CUdp::me()->sendto(msg,4,&local);
	getchar();
    return 0;
}

