/*
	A low level driver for UART0 on C8051F580
*/
#include "platform.h"
#include "UART.h"
#include "UART0.h"

static UART_state_t UART0_state;

void UART0_init(unsigned long BAUDRATE)
{
unsigned char SFR_save = SFRPAGE;
#define _	(SYSCLK / BAUDRATE / 4)
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

	UART_state_init(UART0_state);
}

INTERRUPT(UART0_ISR, INTERRUPT_UART0)
{
	if (RI0 == 1)
	{
		RI0 = 0;
		UART_receive(UART0_state) = SBUF0;
	}
	if (TI0 == 1)
	{
		TI0 = 0;
		if (UART0_state.Written == UART0_state.Sent)
			UART0_state.TX_idle = 1;
		else
			SBUF0 = UART_transmit(UART0_state);
	}
}

void UART0_tx(UART_state_t *const p)
{	// trigger TX interrupt if not already sending
	if (!p->TX_idle == 1)
		return;
	p->TX_idle = 0;
	SBUF0 = pUART_transmit(p);
}

void UART0_write(unsigned char * buffer, unsigned char length)
{
	while (length--)
		UART_write(UART0_state) = *buffer++;
	UART0_tx(&UART0_state);
}

int UART0_pending(void) { return UART_rx_pending(UART0_state); }

unsigned char UART0_read_byte(void) { return UART_read(UART0_state); }
