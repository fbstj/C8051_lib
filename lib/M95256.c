/*
	A driver for M95256-WMN6P SPI EEPROM
*/
#include "platform.h"
#include "SPI0.h"
#include "M95256.h"

enum M95256_INSTRUCTIONS {
	// write enable/disable
	e_M95256_WREN = 0x06, e_M95256_WRDI = 0x04,
	// read/write status register
	e_M95256_RDSR = 0x05, e_M95256_WRSR = 0x01,
	// read/write memory
	e_M95256_READ = 0x03, e_M95256_WRITE = 0x02
};

void M95256_read(const SPI0_device device, short address, unsigned char *const buffer, unsigned char length)
{
unsigned char i;
	length %= M95256_PAGE_SIZE;
	SPI0_select(device);
	SPI0_byte(e_M95256_READ);
	SPI0_byte(address >> 8);
	SPI0_byte(address);
	for (i = 0; i < length; i++)
	{
		buffer[i] = SPI0_byte(0);
	}
	SPI0_select(0);
}

void M95256_write(const SPI0_device device, short address, unsigned char *const buffer, unsigned char length)
{
unsigned char i;
	length %= M95256_PAGE_SIZE;
	SPI0_select(device);
	SPI0_byte(e_M95256_WREN);
	SPI0_select(0);
	i = 1; while (i++) NOP();
	SPI0_select(device);
	SPI0_byte(e_M95256_WRITE);
	SPI0_byte(address >> 8);
	SPI0_byte(address);
	for (i = 0; i < length; i++)
	{
		SPI0_byte(buffer[i]);
	}
	SPI0_select(0);
	i = 1; while (i++) NOP();
	SPI0_select(device);
	SPI0_byte(e_M95256_WRDI);
	SPI0_select(0);
	i = 1; while (i++) NOP();
	SPI0_select(device);
	SPI0_byte(e_M95256_RDSR);
	while (SPI0_byte(0) & 3) ;
	SPI0_select(0);
}