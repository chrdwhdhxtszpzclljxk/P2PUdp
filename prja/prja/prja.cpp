// prja.cpp : �������̨Ӧ�ó������ڵ㡣
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

