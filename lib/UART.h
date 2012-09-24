/*
	UART specific types and enumerations
*/
#ifndef __UART_H
#define __UART_H
enum E_UART_PARITY {
	e_UART_P_NONE	= 0x10,
	e_UART_P_EVEN	= 0x00,
	e_UART_P_ODD	= 0x04,
	e_UART_P_MARK	= 0x0C,
	e_UART_P_SPACE	= 0x08
};

enum E_UART_STOPBITS {
	e_UART_STOP_1	= 0x00,	// one stop bit
	e_UART_STOP_1_5	= 0x01,	// 1.5 stop bits
	e_UART_STOP_2	= 0x02,	// 2 stop bits
	e_UART_STOP_9_6	= 0x03	// 9/6 stop bits
};

enum E_UART_HANDSHAKE {
	e_UART_H_XONXOFF,
	e_UART_H_RTS_CTS,
	e_UART_H_DTR_DSR,
	e_UART_H_NONE_HIGH,
	e_UART_H_NONE_LOW
};

typedef struct T_UART_STATE {
	unsigned char RX[256], Received, Read;
	unsigned char TX[256], Written, Sent;
	char TX_idle, RX_idle;
} UART_state_t;

struct T_UART_CONF {
	unsigned long Baudrate;
	unsigned char Data_Bits;
	enum E_UART_PARITY Parity;
	enum E_UART_STOPBITS Stop_Bits;
	enum E_UART_HANDSHAKE Handshake;
};

#define UART_state_init(p)	p.Read = p.Received = p.Written = p.Sent = 0; p.TX_idle = 1
#define UART_tx_pending(p)	(int)p.Sent - (int)p.Written
#define UART_transmit(p)	p.TX[p.Sent++]
#define UART_write(p)		p.TX[p.Written++]
#define UART_rx_pending(p)	(int)p.Received - (int)p.Read
#define UART_read(p)		p.RX[p.Read++]
#define UART_receive(p)		p.RX[p.Received++]

#define pUART_state_init(p)	p->Read = p->Received = p->Written = p->Sent = 0; p->TX_idle = 1
#define pUART_tx_pending(p)	(int)p->Sent - (int)p->Written
#define pUART_transmit(p)	p->TX[p->Sent++]
#define pUART_write(p)		p->TX[p->Written++]
#define pUART_rx_pending(p)	(int)p->Received - (int)p->Read
#define pUART_read(p)		p->RX[p->Read++]
#define pUART_receive(p)	p->RX[p->Received++]

#endif	// __UART_H