#pragma once
#include <vector>

class client
{
public:
	client();
	~client();
public:
	std::vector<char> mi, mo;
	uint64_t id;
};

