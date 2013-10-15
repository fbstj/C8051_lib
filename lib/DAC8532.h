/*
	DAC8532 SPI digital to analogue converter
*/
#ifndef __DAC8532_H
#define __DAC8532_H

#define DAC8532_pins(name, cs, byte) \
	void name##_A(unsigned int value)\
	{\
		cs = 0;\
		byte(0x10);\
		byte(value >> 8);\
		byte(value);\
		cs = 1;\
	}\
	void name##_B(unsigned int value)\
	{\
		cs = 0;\
		byte(0x24);\
		byte(value >> 8);\
		byte(value);\
		cs = 1;\
	}

#endif //__DAC8532_H