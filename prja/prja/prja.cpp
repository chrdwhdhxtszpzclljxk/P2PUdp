// prja.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\..\classes\Udp.h"

int main()
{
	CUdp::setup(1233);
	CUdp::me();
	getchar();
    return 0;
}

