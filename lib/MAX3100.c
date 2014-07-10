/*
	MAX3100 SPI UART driver (for inbuilt peripheral SPI0)
*/
#include "platform.h"
#include "MAX3100.h"

enum E_MAX3100 {
	CONFIGURE = 0xC000, CONFIGURATION = 0x4000,
	SEND = 0x8000, READ = 0x0000, BAUD = 0x000F,
	TE_RA_FE = 0x0400, RTS_CTS = 0x0200, PARITY = 0x0100,
	TX_PENDING = 0x4000, RX_PENDING = 0x8000
};

// rounds down to the nearest usable baud rate, defaults to 230400
char MAX3100_baud(const long baud)
{
#define CASE(o, n) if (baud >= o) return n;
	CASE(230400,0)
	CASE(115200,1)
	CASE(76800, 2)
	CASE(57600, 3)
	CASE(38400, 4)
	CASE(28800, 5)
	CASE(19200, 6)
	CASE(14400, 7)
	CASE(9600,  8)
	CASE(7200,  9)
	CASE(4800, 10)
	CASE(3600, 11)
	CASE(2400, 12)
	CASE(1800, 13)
	CASE(1200, 14)
	CASE(600,  15)
	return 0;
#undef CASE
}

/* example word function for SPI0
unsigned int MAX3100_word(unsigned int out)
{
unsigned int in;
	SPI0_select(_MAX3100);
	in = SPI0_byte(out >> 8);
	in = SPI0_byte(out) | in << 8;
	SPI0_select(0);
	return in;
}
*/

void MAX3100_init(struct MAX3100 * const self, const enum E_MAX3100_CONFIGURATION conf, const long baud)
{
	self->word(CONFIGURE | (conf & ~(CONFIGURE | BAUD)) | MAX3100_baud(baud));
	UART_clear(&self->RX);
	UART_clear(&self->TX);
}

#define MAX3100_RX_PEND ((self->word(CONFIGURATION) & RX_PENDING) != 0)
#define MAX3100_TX_PEND ((self->word(CONFIGURATION) & TX_PENDING) == 0)

void MAX3100_irq(struct MAX3100 * const self)
{
	while (MAX3100_RX_PEND)	// read pending
		self->RX.buffer[self->RX.head++] = self->word(READ);
}

void MAX3100_send(struct MAX3100 * const self)
{
	MAX3100_irq(self);
	while (self->TX.tail != self->TX.head)
	{	// for all sendable bytes
		MAX3100_irq(self);
		if (MAX3100_TX_PEND) 	// write pending
			continue;
		self->word(SEND | self->TX.buffer[self->TX.tail++]);
	}
	MAX3100_irq(self);
}

int MAX3100_puts(struct MAX3100 * const self, const char * string, int length)
{
int i;

	for (i = 0; i < length; i++)
		self->TX.buffer[self->TX.head++] = string[i];
	MAX3100_send(self);
	return i;
}
