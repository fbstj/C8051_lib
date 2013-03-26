/*
	MAX3100 SPI UART driver
*/
#ifndef __MAX3100_H
#define __MAX3100_H
#include "buffers.h"

/* configuration variables for MAX3100 chips
e_MAX3100_FEN		FIFO enable(d)
e_MAX3100_SHDN		Shutdown mode
e_MAX3100_TM		Enable transmit interrupt
e_MAX3100_RM		Enable receive interrupt
e_MAX3100_PM		Enable parity interrupt
e_MAX3100_RAM		Receiver Activitiy / Framing Error Mask
e_MAX3100_IrDA		IrDA timing mode
e_MAX3100_ST		Stop bit
e_MAX3100_PE		Parity Enable
e_MAX3100_WL		word length
*/
enum E_MAX3100_CONFIGURATION {
	e_MAX3100_FEN = 0x2000, e_MAX3100_SHDN = 0x1000,
	e_MAX3100_TM = 0x0800,e_MAX3100_RM = 0x0400, e_MAX3100_PM = 0x0200, e_MAX3100_RAM = 0x0100,
	e_MAX3100_IrDA = 0x0080, e_MAX3100_ST = 0x0040, e_MAX3100_PE = 0x0020, e_MAX3100_WL = 0x0010
};

struct MAX3100 {
	// exchange a word with the device (including chip selection)
	unsigned int (*word)(unsigned int);
	// data buffers
	struct byte_ring RX, TX;
};

// initialises the MAX3100 chip with the configuration
extern void MAX3100_init(struct MAX3100 * const, const enum E_MAX3100_CONFIGURATION, const long baud);

// service the device (blocking)
// - write all tx buffer to the chip
// - fill rx with any pending data
extern void MAX3100_isr(struct MAX3100 * const);

// add a string to be sent next time ISR gets called
extern void MAX3100_puts(struct MAX3100 * const, char *, int);

// check for characters
#define MAX3100_pending(self)	RING_count(&self->RX)

// read a string out of the buffer
#define MAX3100_getc(self)		RING_get(&self->RX)


#endif	// __MAX3100_H