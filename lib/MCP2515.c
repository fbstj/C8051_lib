/*
	driver for SPI CAN controller MCP2515
*/
#include "platform.h"
#include "MCP2515.h"
#include "MCP2515_regs.h"

#define SELECT(action) self->select(); action; self->deselect()

static unsigned char SPI_get(const struct MCP2515 * const self, unsigned char addr)
{
	SELECT(
		self->byte(CAN_READ);
		self->byte(addr);
		addr = self->byte(0);
		self->byte(0);
	);
	return addr;
}
static void SPI_set(const struct MCP2515 * const self, unsigned char const addr, unsigned char const value )
{
	SELECT(
		self->byte(CAN_WRITE);
		self->byte(addr);
		self->byte(value);
	);
}

#define set(n, v)	SPI_set(self, n, v)
#define get(n)		SPI_get(self, n)

void MCP2515_send(const struct MCP2515 * const self, const struct CAN_frame * const frame)
{
unsigned char txctrl, cRet;
	//Read TX Status
	txctrl = get(TXB0CTRL);

	//Has the previous message sent yet?  If not, resolve this before proceeding
	if (txctrl & TXREQ_SET)
	{	//Then the previous message has *not* sent yet...
		do
		{
			//Was there an error?
			if (txctrl & 0x10)	// Last message started to transmit, but encountered an error condition
				cRet = 0;
			if (txctrl & 0x20)	// Message lost arbitration while being sent (message was lost)
				cRet = -1;

			if (cRet == 1)			// No error, the message is just busy sending itself - re-poll
				txctrl = get(TXB0CTRL);
			else					// Error of some kind: clear it
				set(txctrl, 0);
		}
		while ((txctrl & TXREQ_SET) && (cRet == 1));
	}

	set(TXB0EID8, frame->ID >> 19);
	set(TXB0EID0, frame->ID >> 11);
	set(TXB0SIDH, frame->ID >> 3);
	set(TXB0SIDL, ((frame->ID << 5) & 0xC0) | (frame->Extended == 1 ? 8 : 0) | ((frame->ID >> 27) && 3));

	set(TXB0DLC, (frame->Length & 0xF) | (frame->Remote == 1 ? 0x40 : 0));
	set(TXB0D0, frame->Data[0]); SPI_set(self, TXB0D1, frame->Data[1]);
	set(TXB0D2, frame->Data[2]); SPI_set(self, TXB0D3, frame->Data[3]);
	set(TXB0D4, frame->Data[4]); SPI_set(self, TXB0D5, frame->Data[5]);
	set(TXB0D6, frame->Data[6]); SPI_set(self, TXB0D7, frame->Data[7]);

	set(TXB0CTRL, TXREQ_SET | TXP_INTER_HIGH);

	//Initiate send
	self->select();
	self->byte(CAN_RTS_TXB0);
	self->deselect();
}

char MCP2515_read(const struct MCP2515 * const self, struct CAN_frame * const frame)
{
static struct {
	char ctrl, sidh, sidl, eid8, eid0, dlc;
	char * D;
} _;
unsigned char dummy;
unsigned long arb;
	if (self->irq() == 0)
		return 0;
	_.D = frame->Data;
	// read from correct buffer
	dummy = get(CANSTAT) >> 1;
	switch (dummy & 7)
	{
	case 6:	// read RXB0 into frame
		_.ctrl = get(RXB0CTRL);
		_.sidh = get(RXB0SIDH); _.sidl = get(RXB0SIDL);
		_.eid8 = get(RXB0EID8); _.eid0 = get(RXB0EID0);
		_.dlc  = get(RXB0DLC);
		_.D[0] = get(RXB0D0); _.D[1] = get(RXB0D1);
		_.D[2] = get(RXB0D2); _.D[3] = get(RXB0D3);
		_.D[4] = get(RXB0D4); _.D[5] = get(RXB0D5);
		_.D[6] = get(RXB0D6); _.D[7] = get(RXB0D7);
		break;
	case 7:	// read RXB1 into frame
		_.ctrl = get(RXB1CTRL);
		_.sidh = get(RXB1SIDH); _.sidl = get(RXB1SIDL);
		_.eid8 = get(RXB1EID8); _.eid0 = get(RXB1EID0);
		_.dlc  = get(RXB1DLC);
		_.D[0] = get(RXB1D0); _.D[1] = get(RXB1D1);
		_.D[2] = get(RXB1D2); _.D[3] = get(RXB1D3);
		_.D[4] = get(RXB1D4); _.D[5] = get(RXB1D5);
		_.D[6] = get(RXB1D6); _.D[7] = get(RXB1D7);
		break;
	default:
		return 0;
	}

	arb = _.sidl & 3;
	arb = _.eid8 | (arb << 8);
	arb = _.eid0 | (arb << 8);
	arb = _.sidh | (arb << 8);
	arb = (_.sidl >> 5) | (arb << 3);

	frame->Extended = ((_.sidl  & 8) == 0) ? 0 : 1;
	frame->Remote = ((_.ctrl & 8) == 0) ? 0 : 1;
	frame->ID = arb;
	frame->Length = _.dlc & 0xF;

	// clear SPI CAN interrupts
	set(CANINTF, 0);
	return 1;
}

void MCP2515_baud(const struct MCP2515 * const self, const enum CAN_baud baud)
{
unsigned char old;
	old = get(CANCTRL);
	set(CANCTRL, REQOP_CONFIG);
	switch (baud)
	{
	case CAN_1Mbps:   set(CNF1, 0); break;
	case CAN_500kbps: set(CNF1, 1); break;
	case CAN_250kbps: set(CNF1, 3); break;
	case CAN_125kbps: set(CNF1, 7); break;
	case CAN_100kbps: set(CNF1, 9); break;
	}
	set(CANCTRL, old);
}

void MCP2515_init(const struct MCP2515 * const self)
{
	// enter config mode
	set(CANCTRL, REQOP_CONFIG);

	// configure bit rate and bit timing
	set(CNF2, 0x80 | PHSEG1_3TQ | PRSEG_1TQ);
	set(CNF3, PHSEG2_3TQ);

	// clear receive masks
	set(RXM0SIDH, 0x00);
	set(RXM0SIDL, 0x00);

	set(RXM0EID8, 0x00);
	set(RXM0EID0, 0x00);

	set(RXM1SIDH, 0x00);
	set(RXM1SIDL, 0x00);

	set(RXM1EID8, 0x00);
	set(RXM1EID0, 0x00);

	// clear receve filters
	set(RXF0SIDL, 0x00);
	set(RXF1SIDL, 0x00);

	// enable receive interrupts, disable all other interrupts
	set(CANINTE, RX0IE | RX1IE);

	// clear transmit buffers
	set(TXB0CTRL, 0);

	//Verify device entered Normal mode
	while (OPMODE_NORMAL != (get(CANSTAT) & 0xE0))
		set(CANCTRL, REQOP_NORMAL | CLKOUT_ENABLED | OSM_ENABLED);

	// clear receive buffers
	set(RXB0CTRL, 0x60);
	set(RXB1CTRL, 0x60);
}
