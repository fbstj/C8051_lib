/*
	A low level driver for UART0 on C8051F580
*/
#ifndef __UART0_H
#define __UART0_H

// initialises UART0 with the configuration
extern void UART0_init(unsigned long baudrate);
// write buffer to UART0, return number of bytes written
extern void UART0_write(unsigned char *buffer, unsigned char length);
// byte to receve
extern int UART0_pending(void);
// copy bytes into buffer, return number of bytes read
extern unsigned char UART0_read(unsigned char *buffer, unsigned char length);

#endif // __UART0_H