/*
	A CAN frame structure
*/
#ifndef __CAN_H
#define __CAN_H

struct CAN_frame {
	long ID;				// the message identifier
	unsigned char Data[8];	// the 8 bytes of the packet
	char Length;			// the number of bytes in the packet (DLC field)
	char Extended;			// the frame type (1: 29bit, 0: 11bit)
	char Remote;			// remote frame
};

enum CAN_baud {
	CAN_1Mbps,
	CAN_800kbps,
	CAN_500kbps,
	CAN_250kbps,
	CAN_125kbps,
	CAN_100kbps
};

// initialises a frame
extern void CAN_frame_clear(struct CAN_frame * const);

// copy data between frames
// -- copy(from, to)
extern void CAN_frame_copy(const struct CAN_frame *const, struct CAN_frame *const);

// normalise frame ID
#define CAN_frame_norm_ID(f)	(f->ID & ((f->Extended == 1) ? 0x1FFFFFFF : 0x7FF))

// normalise frame DLC
#define CAN_frame_norm_len(f)	((f->Length > 8) ? 8 : f->Length)

#ifdef CAN0_PAGE

// initialise CAN0
extern void CAN0_init(const enum CAN_baud);

#ifndef CAN0_MO
/* CAN0_basic.c */

// latest frame to be received on CAN0
extern char CAN0_latest(struct CAN_frame * const);

// send a frame and wait for a response
// -- poll(frame to send/fill, timeout)
extern char CAN0_poll(struct CAN_frame * const, long);

// send a message object
extern void CAN0_send(const struct CAN_frame * const);

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

#endif // __CAN_H