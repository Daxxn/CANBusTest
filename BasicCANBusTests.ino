/*
 Name:		BasicCANBusTests.ino
 Created:	5/11/2021 6:53:56 PM
 Author:	Cody
*/

#define testMode 1
#define sender 0

#include <SPI.h>
#include "MCP2515.h"
#include "MCP2515Enums.h"

#define csPin 10

#if testMode == 1
#pragma region MCP CAN Library
MCP2515 CAN = MCP2515(8000000, csPin);

void setup() {
	Serial.begin(115200);

	CAN.SetMode(OPMode::Normal);
	CAN.Begin();
	CAN.Reset();
	delay(200);
#if sender == 0
	CAN.SetMessageFilter(1, 1, 0);
#else
	CAN.SetMessageFilter(1, 1, 1);
	CAN.SetTXID(0b1, 0x31, 8);
#endif // sender == 0

}

void loop() {
#if sender == 0
	Serial.println("Sending");
	CAN.SendData(0b00101010, 1);
#else
	Packet data = CAN.RecieveData();
	PrintBinary(data.ID);
	PrintBinary(data.Data);
#endif // sender == 0
	delay(1000);
}
#pragma endregion
#else
// Direct Test Mode
byte frontData = 0;
byte data = 0;
uint16_t dataLong = 0xFFFF;

byte frame[3] = {};

SPISettings _Settings = SPISettings(8000000, MSBFIRST, SPI_MODE0);

void setup() {
	Serial.begin(115200);
	SPI.begin();
	pinMode(csPin, OUTPUT);
	digitalWrite(csPin, HIGH);
	while (!Serial);
	Serial.println();

	/*Serial.println("Print Test");
	PrintBinary((byte)0b11000011);*/

	ResetCAN();
	delay(500);

	//ClockEnableTest();
	//ConfigTest();
	//NormalRunTest();

	Serial.println("Read Config Reg");
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer((byte)Command::Read);
	//SPI.transfer((byte)Register::Control);
	SPI.transfer(0b01111110);
	data = SPI.transfer(0xFF);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
	PrintBinary(frontData);
	PrintBinary(data);
	Serial.println("Read Done");

	//Set to normal mode:
	Serial.println("Write Config Reg");
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer((byte)Command::Write);
	SPI.transfer((byte)Register::Control);
	SPI.transfer(0b00000000);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
	Serial.println("Write Done");

	delay(200);

	Serial.println("Read Config Reg");
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer((byte)Command::Read);
	//SPI.transfer((byte)Register::Control);
	SPI.transfer(0b01111110);
	data = SPI.transfer(0xFF);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
	PrintBinary(data);
	Serial.println("Read Done");

	delay(500);
	Serial.println("Setup Done");
}

void loop() {
	/*ReadINTStatus();
	PrintBinary(data);
	delay(500);*/

	Serial.println("Write TX Buffer Reg");
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer((byte)Command::Write);
	SPI.transfer((byte)Register::TXBBK0);
	SPI.transfer(0b00101010);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
	Serial.println("Write TX Done");

	delay(200);

	Serial.println("RTS TX 0");
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer((byte)Command::RTS + 1);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
	Serial.println("RTS TX Done");

	delay(2000);
}

void ResetCAN() {
	Serial.println("RESET");
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer(Command::Reset);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
}

void ConfigTest() {
	Serial.println("Write Loop Config");
	//SendStart(0b00010100); // Forward??
	SendStart(0b00010100); // Backward??
	Serial.println("Write Done");
	delay(500);
	Serial.println("Read Config Reg");
	ReadConfigReg();
	PrintBinary(data);
	Serial.println("Read Config Done");
	delay(200);
	Serial.println("Read CANStatus Reg");
	ReadCANStatusReg();
	PrintBinary(data);
	Serial.println("Read CANStatus Done");
}

void ClockEnableTest() {
	Serial.println("Write Config");
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	/*SPI.transfer((byte)Command::Write);
	SPI.transfer(0b11110000);
	SPI.transfer(0b00000000);*/
	SPI.transfer((byte)Command::BitMod);
	SPI.transfer(Register::Control);
	SPI.transfer(0b11100000);
	SPI.transfer(0b11100000);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
	Serial.println("Write Done");
	ReadConfigReg();
	PrintBinary(data);
	Serial.println("Clock Test Done");
}

void NormalRunTest() {
	Serial.println("Write Config");
	SendStart(0b01000000);
	//SendStart(0b00000010);
	Serial.println("Write Config Done.");
	delay(500);
	Serial.println("Read Config");
	ReadConfigReg();
	PrintBinary(data);
	Serial.println("Read Config Done");

	Serial.println("Write TX Buffer");
	WriteTXBuffer();
	Serial.println("Write TX Done");
	delay(200);
	Serial.println("Read TX Buffer");
	ReadTXBuffer();
	PrintBinary(data);
	Serial.println("Read TX Done");

	/*delay(500);
	Serial.println("Set Normal Mode");
	SetNormalMode();
	Serial.println("Normal Mode Set");*/

	Serial.println("Write TX Buffer");
	WriteTXBuffer();
	Serial.println("Write TX Done");
	delay(200);

	delay(200);
	Serial.println("TX");
	RequestToSend();
	Serial.println("TX Done");
}

void SendStart(byte reg) {
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer((byte)Command::Write);
	SPI.transfer((byte)Register::Control);
	SPI.transfer(reg);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
}

void SetNormalMode() {
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer((byte)Command::BitMod);
	SPI.transfer((byte)Register::Control);
	SPI.transfer(0b11100000);
	SPI.transfer((byte)OPMode::Normal);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
}

void ReadINTStatus() {
	//SPI.beginTransaction(writeSettings);
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	data = SPI.transfer((byte)Command::ReadStatus);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
}

void ReadRXStatus() {
	//SPI.beginTransaction(writeSettings);
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	data = SPI.transfer((byte)Command::RXStatus);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
}

void ReadConfigReg() {
	//SPI.beginTransaction(writeSettings);
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer((byte)Command::Read);
	SPI.transfer((byte)Register::Control);
	data = SPI.transfer(0xFF);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
}

void WriteTXBuffer() {
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	//SPI.transfer((byte)Command::LoadTXBFR + 0b001);
	SPI.transfer((byte)Command::LoadBFR0);
	SPI.transfer(0b00101010);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
}

void RequestToSend() {
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer((byte)Command::RTS + 0b001);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
}

void ReadTXBuffer() {
	//SPI.beginTransaction(writeSettings);
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer((byte)Command::Read);
	//data = SPI.transfer((byte)Register::TXBFR0);
	SPI.transfer((byte)Register::TXBFR0);
	data = SPI.transfer(0xFF);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
}

void ReadCANStatusReg() {
	SPI.beginTransaction(_Settings);
	digitalWrite(csPin, LOW);
	SPI.transfer(Command::Read);
	SPI.transfer(Register::Status);
	data = SPI.transfer(0xFF);
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
}
#endif

void PrintBinary(byte input) {
	/*String output = "";
	for (byte i = 0; i < 8; i++)
	{
		output.concat(bitRead(input, i));
	}
	Serial.println(output);*/
	String output = "";
	for (byte i = 8; i > 0; i--)
	{
		output.concat(bitRead(input, i - 1));
	}
	Serial.println(output);
}

void PrintBinary(uint16_t input) {
	String output = "";
	for (byte i = 16; i > 0; i--)
	{
		output.concat(bitRead(input, i - 1));
	}
	Serial.println(output);
}

void PrintBinary(byte input, bool msbFirst) {
	if (msbFirst)
	{
		String output = "";
		for (byte i = 8; i > 0; i--)
		{
			output.concat(bitRead(input, i - 1));
		}
		Serial.println(output);
	}
	else
	{
		String output = "";
		for (byte i = 0; i < 8; i++)
		{
			output.concat(bitRead(input, i));
		}
		Serial.println(output);
	}
}