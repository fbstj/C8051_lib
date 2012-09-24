/*
	MAX3100 SPI UART driver
*/
#ifndef __MAX3100_H
#define __MAX3100_H

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

// initialises the MAX3100 chip with the configuration
extern void MAX3100_init(const SPI0_device device, const enum E_MAX3100_CONFIGURATION conf, const long baud);

// receive all pending characters on device into state
extern void MAX3100_isr(const SPI0_device device, UART_state_t *const state);
// send all pending characters in state to device for sending
// - also receives any characters between transfers
extern void MAX3100_send(const SPI0_device device, UART_state_t *const state);

#endif	// __MAX3100_H