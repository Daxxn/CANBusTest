// MCP2515.h

#ifndef _MCP2515_h
#define _MCP2515_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MCPSettings.h"
#include "MCP2515Enums.h"
#include <SPI.h>


struct BufferState {
	BufferState() { }
	bool RX0;
	bool RX1;
	bool TX0;
	bool TX1;
	bool TX2;

	bool TXReq0;
	bool TXReq1;
	bool TXReq2;

	int CheckRecieved() {
		if (RX0)
		{
			return 0;
		}
		else if (RX1)
		{
			return 1;
		}
		return -1;
	}
};

struct Packet {
	Packet() {}
	Packet(byte id, byte data) {
		ID = id;
		Data - data;
	}
	byte ID;
	byte Data;
};

class MCP2515 {
public:
	MCP2515(uint32_t clock, int chipSelPin);
	MCP2515(uint32_t clock, int chipSelPin, MCPSettings CANSettings);
	void Begin();
	void SendData(byte data, byte senderId);
	void SetTXID(uint16_t id, byte buffer, byte dataLength);
	Packet RecieveData();
	void CheckForMessage();
	void SetMode(OPMode mode);
	void SetMode(OPMode mode, bool clockEN, ClockScalar scalar);
	void SetMode(OPMode mode, bool abort, bool oneShot, bool clockEN, ClockScalar scalar);
	void SetMessageFilter(byte id, byte mask, byte filter);
	void Reset();

	void AttachInterupt(InteruptType intType, int intPin, void (*inter)());
	void DetachInterupt(byte intType);

	void (*DataRecievedEvent) (Packet data);
private:
	int _csPin;
	SPISettings _SPISettings;
	MCPSettings _CANSettings;
	BufferState _bufferStates;

	byte _RXBuffer[2];
	byte _TXBuffer[3];
	bool _RXBufferState[2];
	bool _TXBufferState[3];

	byte _RXBufferIDs[2];
	byte _TXBufferIDs[3];

	byte _commandFrame[2];
	byte _dataFrame[8];
	InteruptType _activeInterupts[8];

	void BitMod(byte address, byte mask, byte data);
	void Send(Command cmd, byte reg, byte* data, byte size);
	void Send(Command cmd, byte reg, byte data);
	void Send(byte cmd, byte reg, byte data);
	byte Read(Register reg);
	void Recieve(Command cmd, byte reg, byte* outputBuffer, byte size);
	Packet RecieveBuffer(byte buffer);
	byte RecieveBufferNoId(byte buffer);
	void RTS(byte TXBuffer);

	void LoadBuffer(byte cmd, byte id, byte data);

	//void CheckBufferState();
	void GetBufferState();

	byte SelectBuffer(Buffer bfr);

	void CheckForInteruptFlags();
};

#endif

