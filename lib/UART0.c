/*
	A low level driver for UART0 on C8051F580
*/
#include "platform.h"
#include "UARTs.h"

static struct UART_MEM TX0, RX0;
static volatile char TX0_idle, RX0_idle;

void U0_init(unsigned long baud)
{
unsigned char SFR_save = SFRPAGE;
#define _	(SYSCLK / baud / 2)

	SFRPAGE = CONFIG_PAGE;
	if ((_ / 0xFFFF) < 1)
	{	// prescaler 1
		SBRL0 = -_;
		SBCON0 |= 0x03;
	}
	else if ((_ / 0xFFFF) < 4)
	{	// prescaler 4
		SBRL0 = -(_ / 4);
		SBCON0 &= ~0x03;
		SBCON0 |= 0x01;
	}
	else if ((_ / 0xFFFF) < 12)
	{	// prescaler 12
		SBRL0 = -(_ / 12);
		SBCON0 &= ~0x03;
	}
	else
	{	// prescaler 48
		SBRL0 = -(_ / 48);
		SBCON0 &= ~0x03;
		SBCON0 |= 0x02;
	}
	SBCON0 |= 0x40;	// Enable baud rate generator
	SFRPAGE = ACTIVE_PAGE;
	SCON0 = 0x10;	// SCON0: 8-bit variable bit rate clear RI0 and TI0 bits
	TI0 = 1;		// Indicate TX0 ready
	SFRPAGE = SFR_save;

	IE |= 0x10;		// enable UART0 interrupts

	UART_clear(&TX0);
	UART_clear(&RX0);
	TX0_idle = RX0_idle = 1;
}

INTERRUPT(U0_ISR, INTERRUPT_UART0)
{
	if (RI0 == 1)
	{
		RI0 = 0;
		RX0.buffer[RX0.head++] = SBUF0;
	}
	if (TI0 == 1)
	{
		TI0 = 0;
		if (TX0.head == TX0.tail)
			TX0_idle = 1;
		else
			SBUF0 = TX0.buffer[TX0.tail++];
	}
}

int U0_puts(char * buffer, int length)
{
int i;

	for (i = 0; i < length; i++)
		TX0.buffer[TX0.head++] = buffer[i];

	if (TX0_idle == 0)
		return i;
	TX0_idle = 0;

	SBUF0 = TX0.buffer[TX0.tail++];

	return i;
}

int U0_pending() { return UART_size(256, RX0.head, RX0.tail); }

int U0_getc() { return RX0.buffer[RX0.tail++]; }

code const struct device U0 = {
	U0_init, U0_pending, U0_getc, 0, U0_puts
};