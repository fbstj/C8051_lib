/*
	A driver for M95256-WMN6P SPI EEPROM
*/
#include "M95256.h"

enum M95256_INSTRUCTIONS {
	// write enable/disable
	e_M95256_WREN = 0x06, e_M95256_WRDI = 0x04,
	// read/write status register
	e_M95256_RDSR = 0x05, e_M95256_WRSR = 0x01,
	// read/write memory
	e_M95256_READ = 0x03, e_M95256_WRITE = 0x02
};

#define SELECT(action) self->select(); action; self->deselect()

void M95256_load( const struct M95256 * const self, const unsigned int address, unsigned char * const buffer, const unsigned char length)
{
unsigned char i;

	length %= M95256_PAGE_SIZE;

	SELECT(
		self->byte(e_M95256_READ);
		self->byte(address >> 8);
		self->byte(address);
		for (i = 0; i < length; i++)
		{
			buffer[i] = self->byte(0);
		}
	);
}

void M95256_save(const struct M95256 * const self, const unsigned int address, const unsigned char * const buffer, unsigned char length)
{
unsigned char i;
extern void sleep(volatile unsigned long);

	length %= M95256_PAGE_SIZE;

	SELECT(
		self->byte(e_M95256_WREN)
	);

	sleep(256);

	SELECT(
		self->byte(e_M95256_WRITE);
		self->byte(address >> 8);
		self->byte(address);
		for (i = 0; i < length; i++)
		{
			self->byte(buffer[i]);
		}
	);

	sleep(256);

	SELECT(
		self->byte(e_M95256_WRDI)
	);

	sleep(256);

	SELECT(
		self->byte(e_M95256_RDSR);
		while (self->byte(0) & 3) ;
	);
}