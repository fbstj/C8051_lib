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

typedef struct T_UART_STATE{
	unsigned char RX[256], Received, Read;
	unsigned char TX[256], Written, Sent;
	char TX_idle, RX_idle;
} UART_state_t;

typedef struct T_UART_CONF{
	unsigned long Baudrate;
	unsigned char Data_Bits;
	enum E_UART_PARITY Parity;
	enum E_UART_STOPBITS Stop_Bits;
	enum E_UART_HANDSHAKE Handshake;
} UART_config_t;

#endif	// __UART_H