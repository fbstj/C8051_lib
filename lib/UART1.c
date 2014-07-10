/*
	A low level driver for 9-bit UART1 on C8051F580
*/
#include "platform.h"
#include "UARTs.h"

static struct UART_MEM TX1, RX1;
static volatile char TX1_idle, RX1_idle;

void U1_init(unsigned long baud)
{
unsigned char SFR_save = SFRPAGE;
#define _	(SYSCLK / baud / 2)

	SFRPAGE = ACTIVE2_PAGE;
	SCON1 = 0x10;	// 8-bit variable bit rate, no STOP, RX enabled, clear RI1 and TI1
	SFRPAGE = ACTIVE_PAGE;
	if (_/256 < 1)
	{	// prescaler 1
		TH1 = -_;
		CKCON |=  0x08;
	}
	else if (_ / 256 < 4)
	{	// prescaler 4
		TH1 = - (_ / 4);
		CKCON &= ~0x0B;
		CKCON |=  0x01;
	}
	else if (_ / 256 < 12)
	{	// prescaler 12
		TH1 = -(_ / 12);
		CKCON &= ~0x0B;
	}
	else
	{	// prescaler 48
		TH1 = -(_ / 48);
		CKCON &= ~0x0B;
		CKCON |=  0x02;
	}
	TL1 = TH1;		// Init T1
	TMOD &= ~0xF0;	// TMOD: T1 in 8-bit autoreload
	TMOD |= 0x20;
	TR1 = 1;		// START T1

	EIE2 |= 0x08;	// Enable U1 interrupts

	SFRPAGE = SFR_save;

	UART_clear(&TX1);
	UART_clear(&RX1);
	TX1_idle = RX1_idle = 1;
}

INTERRUPT(U1_ISR, INTERRUPT_UART1)
{
	if (RI1 == 1)
	{
		RI1 = 0;
		RX1.buffer[RX1.head++] = SBUF1;
	}
	if (TI1 == 1)
	{
		TI1 = 0;
		if (TX1.head == TX1.tail)
			TX1_idle = 1;
		else
			SBUF1 = TX1.buffer[TX1.tail++];
	}
}

int U1_puts(char * buffer, int length)
{
char SFR_save;
int i;

	for (i = 0; i < length; i++)
		TX1.buffer[TX1.head++] = buffer[i];

	if (TX1_idle == 0)
		return i;
	TX1_idle = 0;

	SFR_save = SFRPAGE;
	SFRPAGE = ACTIVE2_PAGE;
	SBUF1 = TX1.buffer[TX1.tail++];
	SFRPAGE = SFR_save;

	return i;
}

int U1_pending() { return UART_size(256, RX1.head, RX1.tail); }

int U1_getc() { return RX1.buffer[RX1.tail++]; }

code const struct device U1 = {
	U1_init, U1_pending, U1_getc, 0, U1_puts, U1_ISR
};