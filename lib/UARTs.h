/*
	onboard UARTs
*/
#ifndef __UART_H
#define __UART_H

struct UART_MEM { unsigned char buffer[256], head, tail; };
#define UART_size(len, head, tail)	(((len) + (head) - (tail)) % (len))
#define UART_clear(r)	(r)->head = (r)->tail = 0
#define UART_MEM_get(self)	((self)->buffer[(self)->tail++])
#define UART_MEM_pend(self)	UART_size(sizeof((self)->buffer), (self)->head, (self)->tail)
#define UART_MEM_puts(self, str, len) { unsigned char i; for (i = 0; i < (len); i++) (self)->buffer[(self)->head++] = (str)[i]; }

#endif	// __UART_H