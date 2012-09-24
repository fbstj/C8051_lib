/*
	MAX3100 SPI UART driver (for inbuilt peripheral SPI0)
*/
#include "UART.h"
#include "SPI0.h"
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
#define GET_BAUD_SETTING(o, n) if (baud >= o) return n;
	GET_BAUD_SETTING(230400,0)
	GET_BAUD_SETTING(115200,1)
	GET_BAUD_SETTING(76800, 2)
	GET_BAUD_SETTING(57600, 3)
	GET_BAUD_SETTING(38400, 4)
	GET_BAUD_SETTING(28800, 5)
	GET_BAUD_SETTING(19200, 6)
	GET_BAUD_SETTING(14400, 7)
	GET_BAUD_SETTING(9600,  8)
	GET_BAUD_SETTING(7200,  9)
	GET_BAUD_SETTING(4800, 10)
	GET_BAUD_SETTING(3600, 11)
	GET_BAUD_SETTING(2400, 12)
	GET_BAUD_SETTING(1800, 13)
	GET_BAUD_SETTING(1200, 14)
	GET_BAUD_SETTING(600,  15)
	return 0;
#undef GET_BAUD_SETTING
}

static SPI0_device _MAX3100;

unsigned int MAX3100_word(unsigned int out)
{
unsigned int in;
	SPI0_select(_MAX3100);
	in = SPI0_byte(out >> 8);
	in = SPI0_byte(out) | in << 8;
	SPI0_select(0);
	return in;
}

void MAX3100_init(const SPI0_device device, const enum E_MAX3100_CONFIGURATION conf, const long baud)
{
	_MAX3100 = device;
	MAX3100_word(CONFIGURE | (conf & ~(CONFIGURE | BAUD)) | MAX3100_baud(baud));
}

#define MAX3100_RX_PEND ((MAX3100_word(CONFIGURATION) & RX_PENDING) != 0)
#define MAX3100_TX_PEND ((MAX3100_word(CONFIGURATION) & TX_PENDING) == 0)

void MAX3100_isr(const SPI0_device device, UART_state_t *const state)
{
	_MAX3100 = device;
	while (MAX3100_RX_PEND)
		pUART_receive(state) = MAX3100_word(READ);
}

void MAX3100_send(const SPI0_device device, UART_state_t *const state)
{
	_MAX3100 = device;
	while (pUART_tx_pending(state))
	{	// for all sendable bytes
		while (MAX3100_RX_PEND)
			pUART_receive(state) = MAX3100_word(READ);
		while (MAX3100_TX_PEND)
		{
			NOP();
			device = 0;
		}
		MAX3100_word(SEND | pUART_send(state));
	}
}
