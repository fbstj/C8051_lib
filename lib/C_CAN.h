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

// initialise CAN0
extern void CAN0_init(void);

// latest frame to be received on CAN0
extern CAN_frame_t *CAN0_latest;

// send a message object
extern void CAN0_send(CAN_frame_t *const frame);

// receive frames without using the interrupt
extern CAN_frame_t *CAN0_poll(void);

#endif	// CAN0_PAGE

#endif // __C_CAN_H