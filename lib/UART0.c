/*
	A low level driver for UART0 on C8051F580
*/
#include "platform.h"
#include <string.h>
#include "UART.h"
#include "UART0.h"

static UART_state_t UART0_state;

void UART0_init(unsigned long BAUDRATE)
{
unsigned char SFR_save = SFRPAGE;
unsigned long _ = SYSCLK / BAUDRATE / 4;
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

	SBCON0 |= 0x40;		// Enable baud rate generator

	SFRPAGE = ACTIVE_PAGE;

	SCON0 = 0x10;	// SCON0: 8-bit variable bit rate clear RI0 and TI0 bits

	TI0 = 1;		// Indicate TX0 ready

	SFRPAGE = SFR_save;

	memset(&UART0_state, 0, sizeof(struct T_UART_STATE));
	UART0_state.TX_idle = 1;
}

INTERRUPT(UART0_ISR, INTERRUPT_UART0)
{
	if (RI0)
	{
		RI0 = 0;
		UART0_state.RX[UART0_state.Received++] = SBUF0;
	}
	if (TI0)
	{
		TI0 = 0;
		if (UART0_state.Written == UART0_state.Sent)
			UART0_state.TX_idle = 1;
		else
			SBUF0 = UART0_state.TX[UART0_state.Sent++];
	}
}

void UART0_write(unsigned char * buffer, unsigned char length)
{
	while (length--)
		UART0_state.TX[UART0_state.Written++] = *buffer++;

	// trigger TX interrupt if not already doing
	if (UART0_state.TX_idle)
	{
		UART0_state.TX_idle = 0;
		SBUF0 = UART0_state.TX[UART0_state.Sent++];
	}
}

int UART0_pending(void) { return (int)UART0_state.Received - (int)UART0_state.Read; }

unsigned char UART0_read(unsigned char *buffer, unsigned char length)
{
unsigned char i = 0;
	while (length -- && UART0_pending() > 0)
	{
		*buffer++ = UART0_state.RX[UART0_state.Read++];
		i++;
	}
	return i;
}

