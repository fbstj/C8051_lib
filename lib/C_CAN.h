/*
	A driver for the CAN bus on C8051F580s
*/
#ifndef __C_CAN_H
#define __C_CAN_H

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

// normalise frame DLC
#define CAN_frame_norm_len(f)	{ if(f->Length > 8)	f->Length = 8; }

#ifdef CAN0_PAGE	// only defined if the processor has a C_CAN device

#define CAN0_MO_COUNT	32	// number of avaliable message objects

// initialise CAN0
extern void CAN0_init(void);

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

#endif	// CAN0_PAGE

#endif // __C_CAN_H