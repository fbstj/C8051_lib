/*
	MCP2511 SPI CAN controller library
*/
#ifndef __MCP2515_H
#define __MCP2515_H

#include "CAN_bus.h"
#include "SPI.h"

// initialises the spi device as a MCP2515 device
extern void MCP2515_init(spi_pt);
// set the spi device to the baud rate passed
extern void MCP2515_baud(spi_pt, const enum CAN_baud);
// check for a new frame
extern char MCP2515_read(spi_pt, struct CAN_frame * const);
// send the passed frame
extern void MCP2515_send(spi_pt, const struct CAN_frame * const);

#define MCP2515_device(name, self, irq, reset)\
	static void name##_init(enum CAN_baud b) { reset = 0; reset = 1; MCP2515_init(&self); MCP2515_baud(&self, b); }\
	static int name##_read(struct CAN_frame *f) { if (irq == 0) return 0; return MCP2515_read(&self, f); }\
	static int name##_send(struct CAN_frame *f) { MCP2515_send(&self, f); return 1; }\
	code const struct device name = {\
		name##_init, 0, name##_read, 0, name##_send\
	};

#endif // __MCP2515_H
