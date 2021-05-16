// 
// 
// 

#include "MCP2515.h"
#include "MCPSettings.h"

MCP2515::MCP2515(uint32_t clock, int chipSelPin)
{
	_csPin = chipSelPin;
	_SPISettings = SPISettings(clock, MSBFIRST, SPI_MODE0);
	_CANSettings = MCPSettings();
}

MCP2515::MCP2515(uint32_t clock, int chipSelPin, MCPSettings CANSettings)
{
	_SPISettings = SPISettings(clock, MSBFIRST, SPI_MODE0);
	_csPin = chipSelPin;
	_CANSettings = CANSettings;
}

void MCP2515::Begin()
{
	pinMode(_csPin, OUTPUT);
	digitalWrite(_csPin, HIGH);
	SPI.begin();
}

void MCP2515::SendData(byte data, byte senderId)
{
	byte bfrId = SelectBuffer(Buffer::TX);
	LoadBuffer(Command::LoadBFR + bfrId, senderId, data);
}

void MCP2515::SetTXID(uint16_t id, byte buffer, byte dataLength)
{
	if (id > 0x7FF) return;
	if (dataLength > 8) return;
	//Send(Command::Write, (0x31 + (buffer << 4)) + 40, (id << 29) + dataLength);
	Send(Command::Write, 0x31 + (buffer << 4) + 10, id);
	Send(Command::Write, 0x31 + (buffer << 4) + 39, dataLength);
}

Packet MCP2515::RecieveData()
{
	byte bfrId = SelectBuffer(Buffer::RX);
	return RecieveBuffer(bfrId);
}

//byte MCP2515::RecieveData(byte data)
//{
//	return byte();
//}

byte MCP2515::Read(Register reg)
{
	byte data = 0;
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(reg);
	data = SPI.transfer(0xFF);
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
	return data;
}

void MCP2515::CheckForMessage()
{
	GetBufferState();

	if (int buffer = _bufferStates.CheckRecieved() != -1)
	{
		DataRecievedEvent(RecieveBuffer(buffer + 1));
	}
}

void MCP2515::SetMode(OPMode mode)
{
	//Send(Command::Write, Register::Control, mode << 5);
	BitMod(Register::Control, 0b11100000, mode << 5);
}

void MCP2515::SetMode(OPMode mode, bool clockEN, ClockScalar scalar)
{
	byte config = mode << 5;
	config = (clockEN << 4) + config;
	config += scalar;
	Send(Command::Write, Register::Control, config);
}

void MCP2515::BitMod(byte reg, byte mask, byte data)
{
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(reg);
	SPI.transfer(mask);
	SPI.transfer(data);
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
}

void MCP2515::Send(Command cmd, byte reg, byte* data, byte size)
{
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(cmd);
	SPI.transfer(reg);
	SPI.transfer(data, size);
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
}

void MCP2515::Send(Command cmd, byte address, byte data)
{
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(cmd);
	SPI.transfer(address);
	SPI.transfer(data);
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
}

void MCP2515::Send(byte cmd, byte reg, byte data)
{
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(cmd);
	SPI.transfer(reg);
	SPI.transfer(data);
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
}

void MCP2515::Recieve(Command cmd, byte address, byte* outputBuffer, byte size)
{
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(cmd);
	SPI.transfer(address);
	for (byte i = 0; i < size; i++)
	{
		outputBuffer[i] = SPI.transfer(0xFF);
	}
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
}

Packet MCP2515::RecieveBuffer(byte buffer)
{
	Packet p = Packet();
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(Command::ReadBFR + (buffer << 1));
	p.ID = SPI.transfer(0xFF);
	p.Data = SPI.transfer(0xFF);
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
	return p;
}

byte MCP2515::RecieveBufferNoId(byte buffer)
{
	byte data = 0;
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(Command::ReadBFR + (buffer << 1));
	data = SPI.transfer(0xFF);
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
	return data;
}

void MCP2515::RTS(byte TXBuffer)
{
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(Command::RTS + 1 << TXBuffer);
}

void MCP2515::LoadBuffer(byte cmd, byte id, byte data)
{
	byte idOut = id >> 2;
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(cmd);
	SPI.transfer(idOut);
	SPI.transfer(data);
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
}

void MCP2515::GetBufferState()
{
	byte _states = 0;
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(Command::ReadStatus);
	_states = SPI.transfer(0xFF);
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();

	if (_states != 0)
	{
		_bufferStates.RX0 = bitRead(_states, 0);
		_bufferStates.RX1 = bitRead(_states, 1);
		_bufferStates.TXReq0 = bitRead(_states, 2);
		_bufferStates.TX0 = bitRead(_states, 3);
		_bufferStates.TXReq1 = bitRead(_states, 4);
		_bufferStates.TX1 = bitRead(_states, 5);
		_bufferStates.TXReq2 = bitRead(_states, 6);
		_bufferStates.TX2 = bitRead(_states, 7);
	}
}

byte MCP2515::SelectBuffer(Buffer bfr)
{
	if (bfr == Buffer::RX)
	{
		if (_bufferStates.RX0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		if (_bufferStates.TX0)
		{
			return 0;
		}
		else if (_bufferStates.TX1)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
}

//void MCP2515::CheckBufferState()
//{
//
//}

void MCP2515::SetMode(OPMode mode, bool abort, bool oneShot, bool clockEN, ClockScalar scalar)
{
	byte data = mode;
	data = (data << 1) + abort;
	data = (data << 1) + oneShot;
	data = (data << 1) + clockEN;
	data = (data << 2) + scalar;
	Send(Command::Write, Register::Control, data);
}

void MCP2515::SetMessageFilter(byte id, byte mask, byte filter)
{
	if (id == 0)
	{
		Send(Command::Write, Register::RX0Filter, filter);
		Send(Command::Write, Register::RX0Mask, mask);
	}
	else if (id == 1)
	{
		Send(Command::Write, Register::RX1Filter, filter);
		Send(Command::Write, Register::RX0Mask, mask);
	}
	else if (id == 2)
	{
		Send(Command::Write, Register::RX2Filter, filter);
		Send(Command::Write, Register::RX0Mask, mask);
	}
	else if (id == 3)
	{
		Send(Command::Write, Register::RX3Filter, filter);
		Send(Command::Write, Register::RX0Mask, mask);
	}
	else if (id == 4)
	{
		Send(Command::Write, Register::RX4Filter, filter);
		Send(Command::Write, Register::RX0Mask, mask);
	}
	else if (id == 5)
	{
		Send(Command::Write, Register::RX5Filter, filter);
		Send(Command::Write, Register::RX0Mask, mask);
	}
}

void MCP2515::Reset()
{
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_csPin, LOW);
	SPI.transfer(Command::Reset);
	digitalWrite(_csPin, HIGH);
	SPI.endTransaction();
}

#pragma region Interupts for later..
void MCP2515::AttachInterupt(InteruptType intType, int intPin, void(*inter)())
{
	attachInterrupt(digitalPinToInterrupt(intPin), inter, FALLING);
}

void MCP2515::DetachInterupt(byte intPin)
{
	detachInterrupt(intPin);
}

void MCP2515::CheckForInteruptFlags()
{

}
#pragma endregion

