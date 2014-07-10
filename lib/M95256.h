/*
	A driver for M95256-WMN6P SPI EEPROM
*/
#ifndef __M95256_H
#define __M95256_H

#include "SPI.h"

#define M95256_PAGE_SIZE	64
// the address of the first byte in the page contianing `address`
#define M95256_PAGE(address)	((address) - ((address) % M95256_PAGE_SIZE))

// read bytes into a buffer
//		from `address` to `address + length`
// -- load(self, address, buffer, length);
extern void M95256_load(spi_pt, const unsigned int, unsigned char * const, const unsigned int);

// write bytes from a buffer
//		from `address` to `address + (length % EE_PAGE_SIZE)`
// -- save(self, address, buffer, length)
extern void M95256_save(spi_pt, const unsigned int, const unsigned char * const, unsigned int);

#endif	// __M95256_H