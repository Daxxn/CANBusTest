// 
// 
// 

#include "MCPSettings.h"
#include "MCP2515.h"

MCPSettings::MCPSettings()
{
	Mode = OPMode::Normal;
	ExternClockEN = false;
	ExtClockScalar = ClockScalar::One;
}

MCPSettings::MCPSettings(OPMode mode, bool extCLK, ClockScalar extCLKScl)
{
	Mode = mode;
	ExternClockEN = extCLK;
	ExtClockScalar = extCLKScl;
}

MCPSettings::MCPSettings(bool extCLK, ClockScalar extCLKScl)
{
	Mode = OPMode::Normal;
	ExternClockEN = extCLK;
	ExtClockScalar = extCLKScl;
}

void MCPSettings::SetFilters(Filter* filters)
{
	if (sizeof(filters) != 6) return;
	for (byte i = 0; i < 6; i++)
	{
		RXFilters[i] = filters[i];
	}
}

void MCPSettings::SetFilter(Filter filter, byte index)
{
	if (index > 6) return;
	RXFilters[index] = filter;
}

byte MCPSettings::GetFilterByte(byte index)
{
	if (index > 6) return 0;
	return RXFilters[index].FilterByte;
}

byte MCPSettings::GetConfigByte()
{
	byte temp = 0;
	temp = (Mode << 5) + temp;
	temp = (AbortTrans << 4) + temp;
	temp = (OneShot << 3) + temp;
	temp = (ExternClockEN << 2) + temp;
	temp += ExtClockScalar;
}

void MCPSettings::SetMask(byte bfrNum, byte mask)
{
	if (bfrNum == 0)
	{
		RX0Mask = mask;
	}
	else if (bfrNum == 1)
	{
		RX1Mask = mask;
	}
}

byte MCPSettings::GetMask(byte bfrNum)
{
	if (bfrNum == 0)
	{
		return RX0Mask;
	}
	else if (bfrNum == 1)
	{
		return RX1Mask;
	}
	else return 0;
}
