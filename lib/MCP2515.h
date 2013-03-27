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
	// chip is waiting to be serviced
	char (*irq)();
};

// initialises the spi device as a MCP2515 device
extern void MCP2515_init(const struct MCP2515 * const);
// set the spi device to the baud rate passed
extern void MCP2515_baud(const struct MCP2515 * const, const enum CAN_baud);
// check for a new frame
extern char MCP2515_read(const struct MCP2515 * const, struct CAN_frame * const);
// send the passed frame
extern void MCP2515_send(const struct MCP2515 * const, const struct CAN_frame * const);

#endif // __MCP2515_H