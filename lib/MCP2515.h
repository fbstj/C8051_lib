/*
	MCP2511 SPI CAN controller library
*/
#ifndef __MCP2515_H
#define __MCP2515_H

#include "CAN_bus.h"

struct MCP2515 {
	// chip select
	void (*select)();
	// chip unselect
	void (*deselect)();
	// exchange a word with the device
	unsigned char (*byte)(unsigned char);
};

// initialises the spi device as a MCP2515 device
extern void MCP2515_init(const struct MCP2515 * const);
// set the spi device to the baud rate passed
extern void MCP2515_baud(const struct MCP2515 * const, const enum CAN_baud);
// check for a new frame
extern char MCP2515_read(const struct MCP2515 * const, struct CAN_frame * const);
// send the passed frame
extern void MCP2515_send(const struct MCP2515 * const, const struct CAN_frame * const);

#define MCP2515_device(name, self, irq, reset)\
	static void name##_init(enum CAN_baud b) { reset = 0; reset = 1; MCP2515_init(&self); MCP2515_baud(&self, b); }\
	static char name##_read(struct CAN_frame *f) { if (irq == 0) return 0; return MCP2515_read(&self, f); }\
	static void name##_send(struct CAN_frame *f) { MCP2515_send(&self, f); }\
	code const struct device name = {\
		name##_init, 0, name##_read, 0, name##_send\
	};

#endif // __MCP2515_H