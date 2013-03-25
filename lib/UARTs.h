/*
	onboard UARTs
*/
#ifndef __UART_H
#define __UART_H

/*
	UART0 controls
*/
extern void U0_init(unsigned long baud);
// send a string
extern int U0_puts(char *, int);
// check for pending characters
extern int U0_pending();
// read a byte
extern unsigned char U0_getc();

/*
	UART1 controls
*/
extern void U1_init(unsigned long baud);
// send a string
extern int U1_puts(char *, int);
// check for pending characters
extern int U1_pending();
// read a byte
extern unsigned char U1_getc();

#endif	// __UART_H