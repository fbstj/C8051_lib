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
#define CAN_frame_norm_ID(f)	((f)->ID & (((f)->Extended == 1) ? 0x1FFFFFFF : 0x7FF))

// normalise frame DLC
#define CAN_frame_norm_len(f)	(((f)->Length > 8) ? 8 : (f)->Length)

#endif // __CAN_H