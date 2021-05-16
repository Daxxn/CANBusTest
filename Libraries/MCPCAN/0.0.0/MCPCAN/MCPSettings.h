// MCPSettings.h

#ifndef _MCPSettings_h
#define _MCPSettings_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MCP2515Enums.h"
#include "MCPFilter.h"

class MCPSettings {
public:
	MCPSettings();
	MCPSettings(OPMode mode, bool extCLK, ClockScalar extCLKScl);
	MCPSettings(bool extCLK, ClockScalar extCLKScl);
	void SetFilters(Filter* filters);
	void SetFilter(Filter filter, byte index);

	byte GetFilterByte(byte index);
	byte GetConfigByte();
	void SetMask(byte bfrNum, byte mask);
	byte GetMask(byte bfrNum);

	OPMode Mode;
private:
	bool ExternClockEN;
	bool AbortTrans;
	bool OneShot;
	ClockScalar ExtClockScalar;
	Filter RXFilters[6];
	byte RX0Mask;
	byte RX1Mask;
};

#endif

