/*
	A driver for the CAN bus on C8051F580s
*/
#include "platform.h"
#include "C_CAN.h"

// breaks between write-read pairs
#define CAN_nop()	NOP();NOP();NOP();NOP()

void CAN_frame_clear(CAN_frame_t *const frame)
{
int i = 0;
	frame->ID = 0x7FF;			// lowest priority message ID
	frame->Length = 8;			// 8 bytes in the packert
	for(i = 0; i < 8; i++)
		frame->Data[i] = 0;		// clear all bytes
	frame->Remote = 0;			//	not a remote request frame
	frame->Extended = 0;		//	not an extended frame
}

void CAN_frame_copy(CAN_frame_t *const from, CAN_frame_t *const to)
{
int i = 0;
	to->ID = from->ID;
	to->Length = from->Length;
	for(i = 0; i < 8; i++)
		to->Data[i] = from->Data[i];
	to->Remote = from->Remote;
	to->Extended = from->Extended;
}

// state
static CAN_frame_t CAN0_RX;
CAN_frame_t *CAN0_latest;

#ifdef CAN0_PAGE
// CAN0STAT masks
#define BOff	0x80		// Busoff Status
#define EWarn	0x40		// Warning Status
#define EPass	0x20		// Error Passive
#define RxOk	0x10		// Receive Message Successfully
#define TxOk	0x08		// Transmitted Message Successfully
#define LEC		0x07		// Last Error Code

void CAN0_init(void)
{
unsigned char SFRPAGE_save = SFRPAGE;
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
	BAUD	CAN0BT
	1 MHz	0x1402
	500 kHz	0x2B02
	250 kHz	0x2B05
	100 kHz	0x2B0E
*/
	CAN0BT = 0x1402;	// 1 MHz
//	CAN0BT = 0x2B02;	// 500 kHz
//	CAN0BT = 0x2B05;	// 250 kHz
//	CAN0BT = 0x2B0E;	// 100 kHz
	CAN0CN |= 0x80;					// enable test mode
	CAN_nop();						// wait
	CAN0TST = 0x04;					// enalbe basic mode
	// CAN initalization is complete
	CAN0CN &= ~0x41;				// return to Normal Mode and disable access to bit timing register

	SFRPAGE = SFRPAGE_save;

	CAN_frame_clear(&CAN0_RX);
	CAN0_latest = 0;
}

void CAN0_send(CAN_frame_t *const m)
{
unsigned char SFRPAGE_save = SFRPAGE;
unsigned short _A2, _A1;			// temporary arbitration regs

	SFRPAGE = CAN0_PAGE;

	CAN_frame_norm_len(m);
	// message control register
	CAN0IF1MC = 0x1480 | m->Length;

	// command request and mask registers
	CAN0IF1CR = 0x0000;
	CAN0IF1CM = 0x00F3;

	// arbitration registers
	if (m->Extended == 1)
	{	// extended frame
		m->ID &= 0x1FFFFFFF;
		_A1 = m->ID;			// lower half of id
		_A2 = (m->ID >> 16);	// upper half of id
		_A2 |= 0x4000;		// set extended flag
	}
	else
	{	// standard frame
		m->ID &= 0x000007FF;
		_A1 = 0;
		_A2 = (m->ID << 2);	// id fits in bits 28-18
	}

	if (m->Remote == 1)
		_A2 &= ~0x2000;		// a remote frame
	else
		_A2 |= 0x2000;		// not a remote frame

	CAN0IF1A1 = _A1;
	CAN0IF1A2 = _A2;

	// data registers
	CAN0IF1DA1L = m->Data[0];
	CAN0IF1DA1H = m->Data[1];
	CAN0IF1DA2L = m->Data[2];
	CAN0IF1DA2H = m->Data[3];
	CAN0IF1DB1L = m->Data[4];
	CAN0IF1DB1H = m->Data[5];
	CAN0IF1DB2L = m->Data[6];
	CAN0IF1DB2H = m->Data[7];

	// send message
	CAN0IF1CM = 0x0087;				// set Direction to Write TxRqst
	CAN0IF1CR = 0x8000;				// start command request
	CAN_nop();
	while (CAN0IF1CRH & 0x80) { }	// poll on Busy bit

	SFRPAGE = SFRPAGE_save;
}

INTERRUPT(CAN0_ISR, INTERRUPT_CAN0)
{	// SFRPAGE is set to CAN0_Page automatically when ISR starts
unsigned char status;
long _arb;
CAN_frame_t *m = &CAN0_RX;
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

		m->Extended = (_arb >> 30) & 1;

		m->Remote = (_arb >> 29) & 1;

		if(m->Extended == 1)
			m->ID = (_arb & 0x1FFFFFFF);
		else
			m->ID = ((_arb >> 18) & 0x07FF);

		// retreive the number of bytes in the packet (upto 8)
		m->Length = CAN0IF2MCL;
		CAN_frame_norm_len(m);

		// copy message into CAN0rx;
		m->Data[0] = CAN0IF2DA1L;
		m->Data[1] = CAN0IF2DA1H;
		m->Data[2] = CAN0IF2DA2L;
		m->Data[3] = CAN0IF2DA2H;
		m->Data[4] = CAN0IF2DB1L;
		m->Data[5] = CAN0IF2DB1H;
		m->Data[6] = CAN0IF2DB2L;
		m->Data[7] = CAN0IF2DB2H;

		CAN0_latest = &CAN0_RX;
	}
}

#else	// CAN0_PAGE
#error	CAN0 not defined (check myPlatform includes the correct target definitions)
#endif	// CAN0_PAGE
