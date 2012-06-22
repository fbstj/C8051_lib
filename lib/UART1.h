/*
	A low level driver for UART1 on C8051F580
*/
#ifndef __UART1_H
#define __UART1_H

// initialises UART1 with the configuration
extern void UART1_init(unsigned long baudrate);
// write buffer to UART1, return number of bytes written
extern void UART1_write(unsigned char *buffer, unsigned char length);
// byte to receve
extern int UART1_pending(void);
// retrieve next byte
extern unsigned char UART1_read_byte(void);

#endif // __UART1_H