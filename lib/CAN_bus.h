/*
	A CAN frame structure
*/
#ifndef __CAN_H
#define __CAN_H

typedef struct T_CAN_FRAME {
	long ID;				// the message identifier
	unsigned char Data[8];	// the 8 bytes of the packet
	char Length;			// the number of bytes in the packet (DLC field)
	char Extended;			// the frame type (1: 29bit, 0: 11bit)
	char Remote;			// remote frame
} CAN_frame_t;

// initialises a frame
extern void CAN_frame_clear(CAN_frame_t *const frame);

// copy data between frames
extern void CAN_frame_copy(CAN_frame_t *const from, CAN_frame_t *const to);

// normalise frame ID
#define CAN_frmame_norm_ID(f)	(f->ID & (f->Extended != 0)? 0x1FFFFFFF : 0x7FF)

// normalise frame DLC
#define CAN_frame_norm_len(f)	{ if (f->Length > 8)	f->Length = 8; }

#ifdef CAN0_PAGE

// initialise CAN0
extern void CAN0_init(void);

#ifdef CAN0_BASIC
/* basic driver for onboard peripheral CAN0, see CAN0_basic.c */

// latest frame to be received on CAN0
extern CAN_frame_t *CAN0_latest;

// send a message object
extern void CAN0_send(CAN_frame_t *const frame);

#else	// CAN0_BASIC
/* driver for onboard peripheral CAN0 utilising message objects, see CAN0.c */

// disable message object and clear all registers
extern void CAN0_clear(char mo);
// configrue messge object to transmit frames with ID passed
extern void CAN0_tx(char mo, long ID);
// configure message object to receive frames with ID passed
extern void CAN0_rx(char mo, long ID);
// transmit message object with data passed
extern void CAN0_send(char mo, unsigned char Data[8], unsigned char Length);
// retreive frame from message object (0 returned if no message received)
extern CAN_frame_t *CAN0_get(char mo);

// TODO: support extended and remote frames
// TODO: use CAN_frame_t more

#endif	// CAN0_BASIC
#endif	// CAN0_PAGE

#endif // __CAN_H