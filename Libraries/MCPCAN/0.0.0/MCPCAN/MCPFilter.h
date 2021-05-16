// MCPFilter.h

#ifndef _MCPFilter_h
#define _MCPFilter_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct Filter {
	byte FilterByte;
	bool Enable;
	String name;
};

#endif

