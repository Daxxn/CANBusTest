#pragma once

enum Command {
	Write = 0b00000010,
	Read = 0b00000011,
	ReadStatus = 0b10100000,
	BitMod = 0b00000101,

	LoadBFR = 0b01000,
	ReadBFR = 0b10010,

	LoadBFR0 = 0b01000001,
	LoadBFR1 = 0b01000011,
	LoadBFR2 = 0b01000101,

	ReadBFR0 = 0b10010010,
	ReadBFR1 = 0b10010100,

	RXStatus = 0b10110000,

	RTS = 0b10000000,
	Reset = 0b11000000
};

enum OPMode {
	Normal = 0b000,
	Sleep = 0b001,
	LoopBack = 0b010,
	Listen = 0b011,
	Config = 0b100
};

enum ClockScalar {
	One = 1,
	Two = 2,
	Four = 3,
	Eight = 4
};

enum Buffer {
	RX,
	TX
};

enum InteruptType {
	MsgError,
	WakeUp,
	MultiError,
	TX0BufferEmpty,
	TX1BufferEmpty,
	TX2BufferEmpty,
	RX0BufferFull,
	RX1BufferFull,
};

enum Register {
	Control = 0b01111111,
	Status  = 0b01111110,

	TXBFR0  = 0b00110110, // Need to test these.
	TXBFR1  = 0b01000110,
	TXBFR2  = 0b01010110,
	TXBBK0  = 0b00111101,
	TXBBK1  = 0b01001101,
	TXBBK2  = 0b01011101,

	RXBFR0  = 0b01100110,
	RXBFR1  = 0b01110110,
	RXBBK0  = 0b01101101,
	RXBBK1  = 0b01111101,

	RX0Mask = 0b00100000,
	RX1Mask = 0b00100100,

	RX0Filter = 0b0,
	RX1Filter = 0b00000100,
	RX2Filter = 0b00001000,
	RX3Filter = 0b00010000,
	RX4Filter = 0b00010100,
	RX5Filter = 0b00011000
};