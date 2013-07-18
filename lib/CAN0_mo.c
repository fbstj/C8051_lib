/*
	A driver for the CAN bus on C8051F580s which utilises message objects
*/
#include "platform.h"
#ifdef CAN0_MO
#include "CAN0.h"

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
	to->ID = from->ID;
	to->Length = from->Length;
	for(i = 0; i < 8; i++)
		to->Data[i] = from->Data[i];
	to->Remote = from->Remote & 1;
	to->Extended = from->Extended  & 1;
}

#ifdef CAN0_PAGE
// CAN0STAT masks
#define BOff	0x80		// Busoff Status
#define EWarn	0x40		// Warning Status
#define EPass	0x20		// Error Passive
#define RxOk	0x10		// Receive Message Successfully
#define TxOk	0x08		// Transmitted Message Successfully
#define LEC		0x07		// Last Error Code

enum E_MO_COMMAND_FLAGS {
	MaskRegs = 0x40,
	ArbitrationRegs = 0x20,
	ControlRegs = 0x10,
	ClearInterrupt = 0x08,
	Transmit = 0x04,
	ClearNewData = 0x04,
	DataA = 0x02, DataB = 0x01, DataRegs = 0x03,
	WRITE = 0x80, READ = 0x00
};
enum E_MO_CONTROL_FLAGS {
	NewData = 0x8000,
	MessageLost = 0x4000,
	InterruptPending = 0x2000,
	UseMask = 0x1000,
	TransmitInterruptEnable = 0x0800,
	ReceiveInterruptEnable = 0x0400,
	RemoteEnable = 0x0200,	// auto reply to remote frames
	Transmitting = 0x0100,
	EoB = 0x0080
};
enum E_MO_MASK_ARBITRATION_FLAGS {
	EXTENDED_ID = 0x1FFFFFFF,
	STANDARD_ID = 0x000007FF,
	MessageValid = 0x8000,
	Extended = 0x4000,
	DirectionFlag = 0x2000,
	MaskExtended = 0x8000,
	MaskDirection = 0x4000
};

unsigned char FRAME_RX[CAN0_MO_COUNT];

#define MO_norm(mo)	(mo % CAN0_MO_COUNT)
#define IF1_MO(mo)	CAN0IF1CR = MO_norm(mo);	while (CAN0IF1CRH & 0x80)
#define IF2_MO(mo)	CAN0IF2CR = MO_norm(mo);	while (CAN0IF2CRH & 0x80)

void CAN0_init(const enum CAN_baud baud)
{
char i;
unsigned char SFRPAGE_save = SFRPAGE;
	SFRPAGE = CAN0_PAGE;
	CAN0CN = 1;

	CAN0CN |= 0x4E;

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

	CAN0CN &= ~0x41;

	SFRPAGE = SFRPAGE_save;

	for (i = 0; i < CAN0_MO_COUNT; i++)
	{
		CAN0_clear(i);
	}
}

void CAN0_clear(char mo)
{
unsigned char SFRPAGE_save = SFRPAGE;
	SFRPAGE = CAN0_PAGE;

	// clear flags
	CAN0IF1CM = READ | ClearInterrupt | ClearNewData;
	IF1_MO(mo);
	// clear registers
	CAN0IF1A1 = CAN0IF1A2 = 0x0000;
	CAN0IF1M1 = CAN0IF1M2 = 0x0000;
	CAN0IF1MC = EoB;
	CAN0IF1DA1L = CAN0IF1DA1H = CAN0IF1DA2L = CAN0IF1DA2H = 0;
	CAN0IF1DB1L = CAN0IF1DB1H = CAN0IF1DB2L = CAN0IF1DB2H = 0;
	// write the arbitration and control registers
	CAN0IF1CM = WRITE | ControlRegs | ArbitrationRegs | MaskRegs | DataRegs;
	IF1_MO(mo);
	// clear receive counter
	FRAME_RX[MO_norm(mo)] = 0;

	SFRPAGE = SFRPAGE_save;
}

void CAN0_tx(char mo, long ID)
{
unsigned char SFRPAGE_save = SFRPAGE;
	SFRPAGE = CAN0_PAGE;

	// set the arbitration registers
	CAN0IF1A1 = 0x0000;
	CAN0IF1A2 = MessageValid | DirectionFlag | (ID << 2);
	// last message in buffer
	CAN0IF1MC = EoB;
	// write the arbitration and control registers
	CAN0IF1CM = WRITE | ArbitrationRegs | ControlRegs;
	IF1_MO(mo);

	SFRPAGE = SFRPAGE_save;
}

void CAN0_send(const char mo, const unsigned char Data[8], const unsigned char Length)
{
unsigned char SFRPAGE_save = SFRPAGE;
	SFRPAGE = CAN0_PAGE;

	// read CAN0IF1MC for the flags
	CAN0IF1CM = READ | ControlRegs;
	IF1_MO(mo);
	// update data registers
	CAN0IF1DA1L = Data[0]; CAN0IF1DA1H = Data[1];
	CAN0IF1DA2L = Data[2]; CAN0IF1DA2H = Data[3];
	CAN0IF1DB1L = Data[4]; CAN0IF1DB1H = Data[5];
	CAN0IF1DB2L = Data[6]; CAN0IF1DB2H = Data[7];
	// update DLC in control register
	CAN0IF1MC = (CAN0IF1MC & 0xFFF0) | ((Length > 8) ? 8 : Length);
	// write the data and control registers and trigger a transmit
	CAN0IF1CM = WRITE | Transmit | DataRegs | ControlRegs;
	IF1_MO(mo);

	SFRPAGE = SFRPAGE_save;
}

void CAN0_rx(char mo, long ID)
{
unsigned char SFRPAGE_save = SFRPAGE;
	SFRPAGE = CAN0_PAGE;

	// set mask registers to mask for the correct bits
	CAN0IF1M1 = 0x0000;
	CAN0IF1M2 = MaskDirection | (STANDARD_ID << 2);
	// set arbitration registers to match the passed ID
	CAN0IF1A1 = 0x0000;
	CAN0IF1A2 = MessageValid | (ID << 2);
	// use mask registers and enable interrupts
	CAN0IF1MC = UseMask | ReceiveInterruptEnable | EoB;
	// write the arbitration, mask and control registers
	CAN0IF1CM = WRITE | ArbitrationRegs | MaskRegs | ControlRegs;
	IF1_MO(mo);

	SFRPAGE = SFRPAGE_save;
}

INTERRUPT (CAN0_ISR, INTERRUPT_CAN0)
{
char status, mo;
	status = CAN0STAT;
	mo = CAN0IID;

	CAN0IF1CM = READ | ClearInterrupt | ClearNewData;
	IF1_MO(mo);

	if (status & RxOk)
	{	// receive completed successfully
		FRAME_RX[MO_norm(mo)]++;
	}
}

struct CAN_frame * CAN0_get(char mo)
{
static long _arb;
static struct CAN_frame f;
unsigned char SFRPAGE_save = SFRPAGE;
	SFRPAGE = CAN0_PAGE;

	if (FRAME_RX[MO_norm(mo)] == 0)
		return 0;

	CAN0IF1CM = READ | ControlRegs | ArbitrationRegs | DataRegs;
	IF1_MO(mo);

	_arb = CAN0IF1A2;
	_arb = _arb << 16;
	_arb |= CAN0IF1A1;

	f.Extended = (_arb >> 30) & 1;

	f.Remote = (_arb >> 29) & 1;

	if(f.Extended == 1)
		f.ID = (_arb & 0x1FFFFFFF);
	else
		f.ID = ((_arb >> 18) & 0x07FF);

	// retreive the number of bytes in the packet (upto 8)
	f.Length = CAN0IF1MCL & 0x0F;
	if (f.Length > 8)
		f.Length = 8;

	// copy message into CAN0rx;
	f.Data[0] = CAN0IF1DA1L; f.Data[1] = CAN0IF1DA1H;
	f.Data[2] = CAN0IF1DA2L; f.Data[3] = CAN0IF1DA2H;
	f.Data[4] = CAN0IF1DB1L; f.Data[5] = CAN0IF1DB1H;
	f.Data[6] = CAN0IF1DB2L; f.Data[7] = CAN0IF1DB2H;

	SFRPAGE = SFRPAGE_save;

	FRAME_RX[MO_norm(mo)] = 0;

	return &f;
}

#else	// CAN0_PAGE
#error	CAN0 not defined (check platfomr.h includes the correct target definitions)
#endif	// CAN0_PAGE
#endif	// CAN0_MO