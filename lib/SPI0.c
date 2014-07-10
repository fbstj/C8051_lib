/*
	SPI0 driver
*/
#include "platform.h"
#include "SPI0.h"

/*
enum SPI0_registers {
	SPIBSY = 0x80,
	MSTEN = 0x40,
	CKPHA = 0x20,
	CKPOL = 0x10,
	SLVSEL = 0x08,
	NSSIN = 0x04,
	SRMT = 0x02,
	RXBMT = 0x01,
	_SPIF = 0x80,
	WCOL = 0x40,
	MODF = 0x20,
	RXOVRN = 0x10,
	NSSMD_3wire = 0x00,
	NSSMD_4wire = 0x08,
	TXBMT = 0x02,
	SPIEN = 0x01,
};
*/

void SPI0_init(unsigned long bitrate)
{
	SPI0CFG = 0x40;
	SPI0CN = 0x01;
	bitrate = SYSCLK / (2 * bitrate);
	SPI0CKR = bitrate - 1;
}

unsigned char SPI0_byte(unsigned char word)
{
	SPIF = 0;
	SPI0DAT = word;
	while (SPIF == 0) ;
	return SPI0DAT;
}
