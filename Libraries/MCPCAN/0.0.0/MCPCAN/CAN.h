// CAN.h

#ifndef _CAN_h
#define _CAN_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif



struct CANPins {
	int TXbufferIntPins[3];
	int RXBufferIntPins[2];
};

class CAN {

};

#endif

