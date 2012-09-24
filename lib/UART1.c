/*
	A low level driver for 9-bit UART1 on C8051F580
*/
#include "platform.h"
#include "UART.h"
#include "UART1.h"

static UART_state_t STATE;

#ifdef UART1_9bit
extern bit UART1_RX_9bit;
#endif

void UART1_init(unsigned long baud)
{
unsigned char SFR_save = SFRPAGE;
#define _	(SYSCLK / baud / 4)
	SFRPAGE = ACTIVE2_PAGE;
	SCON1 = 0x90;	// 9-bit variable bit rate, no STOP, RX enabled, clear RI1 and TI1
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
	TL1 = TH1;		// Init Timer1
	TMOD &= ~0xF0;	// TMOD: timer 1 in 8-bit autoreload
	TMOD |= 0x20;
	TR1 = 1;		// START Timer1
	EIE2 |= 0x08;	// Enable UART1 interrupts
#ifdef UART1_9bit
	TB81 = 0;		// 9th bit is unset
#endif
	SFRPAGE = SFR_save;
#ifdef UART1_RS485
	UART1_DIR = 0;	// set tranceiver in recieve mode
#endif
#ifdef UART1_9bit
	UART1_RX_9bit = 0;
#endif
	UART_state_init(STATE);
}

INTERRUPT(UART1_ISR, INTERRUPT_UART1)
{
	if (RI1 == 1)
	{
		RI1 = 0;
#ifdef UART1_9bit
		if (RB81 == 1)
		{	// first byte of frame
			STATE.Received = STATE.Read;
			UART1_RX_9bit = 1;
		}
#endif
		UART_receive(STATE) = SBUF1;
	}
	if (TI1 == 1)
	{
		TI1 = 0;
#ifdef UART1_9bit
		TB81 = 0;	// clear the 9th bit for all subsequent bytes
#endif
		if (STATE.Written == STATE.Sent)
		{	// end of transmit message
			STATE.TX_idle = 1;
#ifdef UART1_RS485
			UART1_DIR = 0;	// return tranceiverto recieve mode
#endif
		}
		else
		{
			SBUF1 = UART_transmit(STATE);
		}
	}
}

void UART1_tx(UART_state_t *const p)
{	// trigger TX interrupt if not already sending
unsigned char SFR_save = SFRPAGE;
	if (!p->TX_idle == 1)
		return;
#ifdef UART1_RS485
	UART1_DIR = 1;	// set the tranciever to transmit mode
#endif
	p->TX_idle = 0;
	SFR_save = SFRPAGE;
	SFRPAGE = ACTIVE2_PAGE;
#ifdef UART1_9bit
	TB81 = 1;	// set the 9th bit for the first byte
#endif
	SBUF1 = pUART_transmit(p);
	SFRPAGE = SFR_save;
}

void UART1_write(unsigned char *buffer, unsigned char length)
{
	while (length--)
		UART_write(STATE) = *buffer++;
	UART1_tx(&STATE);
}

int UART1_pending(void) { return UART_rx_pending(STATE); }

unsigned char UART1_read_byte(void) { return UART_read(STATE); }
