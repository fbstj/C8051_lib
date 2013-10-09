/*
	driver for CAN0
*/
#ifndef __CAN0_H
#define __CAN0_H

#include "CAN_bus.h"

#ifdef CAN0_PAGE

// initialise CAN0
extern void CAN0_init(const enum CAN_baud);

#ifndef CAN0_MO
/* CAN0_basic.c */

// latest frame to be received on CAN0
extern int CAN0_latest(struct CAN_frame * const);

// send a frame and wait for a response
// -- poll(frame to send/fill, timeout)
extern int CAN0_poll(struct CAN_frame * const, long);

// send a message object
extern int CAN0_send(const struct CAN_frame * const);

extern code const struct device CAN0;

#else	// CAN0_MO
/* CAN0_mo.c */

#define CAN0_MO_COUNT	32

// disable message object and clear all registers
extern void CAN0_clear(const char mo);
// configrue messge object to transmit frames with ID passed
extern void CAN0_tx(const char mo, const long ID);
// configure message object to receive frames with ID passed
extern void CAN0_rx(const char mo, const long ID);
// transmit message object with data passed
extern void CAN0_send(const char mo, const unsigned char Data[8], const unsigned char Length);
// retreive frame from message object (0 returned if no message received)
extern struct CAN_frame * CAN0_get(const char mo);

#endif	// CAN0_MO

#endif	// CAN0_PAGE

#endif // __CAN0_H