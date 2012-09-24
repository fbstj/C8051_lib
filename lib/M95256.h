/*
	A driver for M95256-WMN6P SPI EEPROM
*/
#ifndef __M95256_H
#define __M95256_H

#define M95256_PAGE_SIZE	64
// the address of the first byte in the page contianing `address`
#define M95256_PAGE(address)	((address) - ((address) % M95256_PAGE_SIZE))

// read bytes into a buffer
//		from `address` to `address + length`
extern void M95256_read(const SPI0_device device, short address, unsigned char *const buffer, unsigned char length);

// write bytes from a buffer
//		from `address` to `address + (length % EE_PAGE_SIZE)`
extern void M95256_write(const SPI0_device device, short address, unsigned char *const buffer, unsigned char length);

#endif	// __M95256_H