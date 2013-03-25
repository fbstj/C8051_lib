/*
	A low level driver for UART0 on C8051F580
*/
#include "platform.h"
#include "buffers.h"
#include "UARTs.h"

static struct byte_ring TX0, RX0;
static volatile char TX0_idle, RX0_idle;

void U0_init(unsigned long baud)
{
unsigned char SFR_save = SFRPAGE;
#define _	(SYSCLK / baud / 4)

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

	RING_clear(&TX0);
	RING_clear(&RX0);
	TX0_idle = RX0_idle = 1;
}

INTERRUPT(U0_ISR, INTERRUPT_UART0)
{
	if (RI0 == 1)
	{
		RI0 = 0;
		RING_put(&RX0) = SBUF0;
	}
	if (TI0 == 1)
	{
		TI0 = 0;
		if (TX0.head == TX0.tail)
			TX0_idle = 1;
		else
			SBUF0 = RING_get(&TX0);
	}
}

int U0_puts(char * buffer, int length)
{
int i;

	for (i = 0; i < length; i++)
		RING_put(&TX0) = buffer[i];

	if (TX0_idle == 0)
		return i;
	TX0_idle = 0;

	SBUF0 = RING_get(&TX0);

	return i;
}

int U0_pending() { return RING_count(&RX0); }

unsigned char U0_getc() { return RING_get(&RX0); }
