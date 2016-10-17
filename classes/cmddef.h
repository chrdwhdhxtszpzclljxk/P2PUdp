#pragma once

#include <stdint.h>


enum {
	init = 0,
	quit = 1
};

struct cmdbase {
	uint8_t cmd;
	uint64_t d;
};

