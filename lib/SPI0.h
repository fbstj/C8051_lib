/*
	driver for SPI0, the onboard Serial Peripheral Interface
*/
#ifndef __SPI0_H
#define __SPI0_H

// initialise SPI0 with the passed bitrate
void SPI0_init(unsigned long bitrate);

// shift a byte onto the bus and read a byte off it
unsigned char SPI0_byte(unsigned char);

#endif // __SPI0_H
