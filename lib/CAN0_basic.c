/*
	A basic-mode driver for the CAN bus on C8051F580s
*/
#include "platform.h"
#include "CAN_bus.h"

// breaks between write-read pairs
#define CAN_nop()	NOP();NOP();NOP();NOP()

void CAN_frame_clear(struct CAN_frame * const frame)
{
int i = 0;
	frame->ID = 0x7FF;			// lowest priority message ID
	frame->Length = 8;			// 8 bytes in the packert
	for(i = 0; i < 8; i++)
		frame->Data[i] = 0;		// clear all bytes
	frame->Remote = 0;			//	not a remote request frame
	frame->Extended = 0;		//	not an extended frame
}

void CAN_frame_copy(const struct CAN_frame * const from, struct CAN_frame * const to)
{
int i = 0;
	to->ID = CAN_frame_norm_ID(from);
	to->Length = CAN_frame_norm_len(from);
	for(i = 0; i < 8; i++)
		to->Data[i] = from->Data[i];
	to->Remote = from->Remote;
	to->Extended = from->Extended;
}

// state
static struct CAN_frame RX;
static volatile char received;

#ifdef CAN0_PAGE
// CAN0STAT masks
#define BOff	0x80		// Busoff Status
#define EWarn	0x40		// Warning Status
#define EPass	0x20		// Error Passive
#define RxOk	0x10		// Receive Message Successfully
#define TxOk	0x08		// Transmitted Message Successfully
#define LEC		0x07		// Last Error Code

void CAN0_init(const enum CAN_baud baud)
{
unsigned char SFR_save = SFRPAGE;
	SFRPAGE = CAN0_PAGE;
	CAN0CN = 1;						// start Intialization mode
	CAN_nop();						// wait
	// initialize general CAN peripheral settings
	CAN0CN |= 0x4E;					// enable Error, Module and access to bit timing register
	CAN_nop();						// wait
/*	CAN0BT bit timing register
	@ 24 MHz CAN_CLK, 80% sample point
http://www.port.de/pages/misc/bittimings.php?lang=en
http://www.port.de/cgi-bin/tq.cgi?ctype=C_CAN&CLK=24&sample_point=80
	BAUD	1MHz	500kHz	250kHz	100kHz
	CAN0BT	0x1402	0x2B02	0x2B05	0x2B0E
*/
	switch (baud)
	{
	case CAN_1Mbps:		CAN0BT = 0x1402;
	case CAN_500kbps:	CAN0BT = 0x2B02;
	case CAN_250kbps:	CAN0BT = 0x2B05;
	case CAN_125kbps:	CAN0BT = 0x2B0B;
	case CAN_100kbps:	CAN0BT = 0x2B0E;
	}

	CAN0CN |= 0x80;					// enable test mode
	CAN_nop();						// wait
	CAN0TST = 0x04;					// enalbe basic mode
	// CAN initalization is complete
	CAN0CN &= ~0x41;				// return to Normal Mode and disable access to bit timing register

	SFRPAGE = ACTIVE_PAGE;

	// enable CAN interrupts
	EIE2 |= 0x02;

	SFRPAGE = SFR_save;

	CAN_frame_clear(&RX);
	received = 0;
}

void CAN0_send(const struct CAN_frame * const f)
{
unsigned char SFR_save = SFRPAGE;
unsigned short _A2, _A1;			// temporary arbitration regs

	SFRPAGE = CAN0_PAGE;

	// message control register
	CAN0IF1MC = 0x1480 | CAN_frame_norm_len(f);

	// command request and mask registers
	CAN0IF1CR = 0x0000;
	CAN0IF1CM = 0x00F3;

	// arbitration registers
	if (f->Extended == 1)
	{	// extended frame
		f->ID &= 0x1FFFFFFF;
		_A1 = f->ID;			// lower half of id
		_A2 = (f->ID >> 16);	// upper half of id
		_A2 |= 0x4000;		// set extended flag
	}
	else
	{	// standard frame
		f->ID &= 0x000007FF;
		_A1 = 0;
		_A2 = (f->ID << 2);	// id fits in bits 28-18
	}

	if (f->Remote == 1)
		_A2 &= ~0x2000;		// a remote frame
	else
		_A2 |= 0x2000;		// not a remote frame

	CAN0IF1A1 = _A1;
	CAN0IF1A2 = _A2;

	// data registers
	CAN0IF1DA1L = f->Data[0];
	CAN0IF1DA1H = f->Data[1];
	CAN0IF1DA2L = f->Data[2];
	CAN0IF1DA2H = f->Data[3];
	CAN0IF1DB1L = f->Data[4];
	CAN0IF1DB1H = f->Data[5];
	CAN0IF1DB2L = f->Data[6];
	CAN0IF1DB2H = f->Data[7];

	// send message
	CAN0IF1CM = 0x0087;				// set Direction to Write TxRqst
	CAN0IF1CR = 0x8000;				// start command request
	CAN_nop();
	while (CAN0IF1CRH & 0x80) { }	// poll on Busy bit

	SFRPAGE = SFR_save;

	received = 0;
}

INTERRUPT(CAN0_ISR, INTERRUPT_CAN0)
{	// SFRPAGE is set to CAN0_Page automatically when ISR starts
unsigned char status;
long _arb;
//unsigned char Interrupt_ID;

	status = CAN0STAT;				// read status, which clears the Status Interrupt bit pending in CAN0IID

	CAN0IF2CM = 0x007F;				// read all of message object to IF2 Clear IntPnd and newData
	CAN_nop();
	while (CAN0IF2CRH & 0x80) {}	// poll on Busy bit

	if (status & RxOk)
	{	// receive completed successfully
		_arb = CAN0IF2A2;
		_arb = _arb << 16;
		_arb |= CAN0IF2A1;

		RX.Extended = (_arb >> 30) & 1;

		RX.Remote = (_arb >> 29) & 1;

		if(RX.Extended == 1)
			RX.ID = (_arb & 0x1FFFFFFF);
		else
			RX.ID = ((_arb >> 18) & 0x07FF);

		// retreive the number of bytes in the packet (upto 8)
		RX.Length = CAN0IF2MCL;

		// copy message into CAN0rx;
		RX.Data[0] = CAN0IF2DA1L;
		RX.Data[1] = CAN0IF2DA1H;
		RX.Data[2] = CAN0IF2DA2L;
		RX.Data[3] = CAN0IF2DA2H;
		RX.Data[4] = CAN0IF2DB1L;
		RX.Data[5] = CAN0IF2DB1H;
		RX.Data[6] = CAN0IF2DB2L;
		RX.Data[7] = CAN0IF2DB2H;

		received = 1;
	}
}

char CAN0_latest(struct CAN_frame * const f)
{
	if (received == 0)
		return 0;
	CAN_frame_copy(&RX, f);
	received = 0;
	return 1;
}

char CAN0_poll(struct CAN_frame * const f, long ticks)
{
	while (received == 0) if (ticks-- == 0) return 0;
	CAN_frame_copy(&RX, f);
	received = 0;
	return 1;
}

#else	// CAN0_PAGE
#error	CAN0 not defined (check platform.h includes the correct target definitions)
#endif	// CAN0_PAGE
