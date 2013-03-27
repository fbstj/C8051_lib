/*
	driver for SPI CAN controller MCP2515
*/
#include "platform.h"
#include "MCP2515.h"
#include "MCP2515_regs.h"

static unsigned char SPI_get(const struct MCP2515 * const self, unsigned char addr)
{
	self->select();
	self->byte(CAN_READ);
	self->byte(addr);
	addr = self->byte(0);
	self->byte(0);
	self->deselect();
	return addr;
}
static void SPI_set(const struct MCP2515 * const self, unsigned char const addr, unsigned char const value )
{
	self->select();
	self->byte(CAN_WRITE);
	self->byte(addr);
	self->byte(value);
	self->deselect(self);
}

void MCP2515_send(const struct MCP2515 * const self, const struct CAN_frame * const frame)
{
unsigned char TXB0CTRLval, cRet;
	//Read TX Status
	TXB0CTRLval = SPI_get(self, TXB0CTRL);

	//Has the previous message sent yet?  If not, resolve this before proceeding
	if (TXB0CTRLval & TXREQ_SET)
	{	//Then the previous message has *not* sent yet...
		do
		{
			//Was there an error?
			if (TXB0CTRLval & 0x10)	// Last message started to transmit, but encountered an error condition
				cRet = 0;
			if (TXB0CTRLval & 0x20)	// Message lost arbitration while being sent (message was lost)
				cRet = -1;

			if (cRet == 1)			// No error, the message is just busy sending itself - re-poll
				TXB0CTRLval = SPI_get(self, TXB0CTRL);
			else					// Error of some kind: clear it
				SPI_set(self, TXB0CTRL, 0);
		}
		while ((TXB0CTRLval & TXREQ_SET) && (cRet == 1));
	}

	SPI_set(self, TXB0SIDH, frame->ID >> 3);
	SPI_set(self, TXB0SIDL, (frame->ID << 5) & 0xC0);
	SPI_set(self, TXB0DLC, frame->Length & 0xF);
	SPI_set(self, TXB0D0, frame->Data[0]); SPI_set(self, TXB0D1, frame->Data[1]);
	SPI_set(self, TXB0D2, frame->Data[2]); SPI_set(self, TXB0D3, frame->Data[3]);
	SPI_set(self, TXB0D4, frame->Data[4]); SPI_set(self, TXB0D5, frame->Data[5]);
	SPI_set(self, TXB0D6, frame->Data[6]); SPI_set(self, TXB0D7, frame->Data[7]);

	SPI_set(self, TXB0CTRL, TXREQ_SET | TXP_INTER_HIGH);

	//Initiate send
	self->select();
	self->byte(CAN_RTS_TXB0);
	self->deselect();
}
char MCP2515_latest(const struct MCP2515 * const self, struct CAN_frame * const frame)
{
unsigned char dummy;
unsigned char arb;
	if (self->irq() == 0)
		return 0;
	// read from correct buffer
	dummy = SPI_get(self, CANSTAT);
	if ((dummy & 0x0E) == 0x0C)
	{	// read RXB0 into frame
		arb = SPI_get(self, RXB0SIDL);
		frame->Extended = 0;
		frame->Remote = 0;
		frame->ID = SPI_get(self, RXB0SIDH);
		frame->ID = arb >> 5 | frame->ID << 3;
		frame->Length = SPI_get(self, RXB0DLC) & 0xF;
		frame->Data[0] = SPI_get(self, RXB0D0);
		frame->Data[1] = SPI_get(self, RXB0D1);
		frame->Data[2] = SPI_get(self, RXB0D2);
		frame->Data[3] = SPI_get(self, RXB0D3);
		frame->Data[4] = SPI_get(self, RXB0D4);
		frame->Data[5] = SPI_get(self, RXB0D5);
		frame->Data[6] = SPI_get(self, RXB0D6);
		frame->Data[7] = SPI_get(self, RXB0D7);
	}
	else if ((dummy & 0x0E) == 0x0F)
	{	// read RXB1 into frame
	}
	else
		return 0;
	// clear SPI CAN interrupts
	SPI_set(self, CANINTF, 0);
	return 1;
}

void MCP2515_baudrate(const struct MCP2515 * const self, const unsigned long baud)
{
unsigned char old;
	old = SPI_get(self, CANCTRL);
	SPI_set(self, CANCTRL, REQOP_CONFIG);
	switch (baud)
	{
	case 1000000: case CAN_1Mbps:	SPI_set(self, CNF1, 0); break;
	case  500000: case CAN_500kbps: SPI_set(self, CNF1, 1); break;
	case  250000: case CAN_250kbps: SPI_set(self, CNF1, 3); break;
	case  125000: case CAN_125kbps: SPI_set(self, CNF1, 7); break;
	case  100000: case CAN_100kbps: SPI_set(self, CNF1, 9); break;
	}
	SPI_set(self, CANCTRL, old);
}

void MCP2515_init(const struct MCP2515 * const self)
{
unsigned char dummy;
	// clear receive masks
	SPI_set(self, RXM0SIDH, 0x00);
	SPI_set(self, RXM0SIDL, 0x00);

	SPI_set(self, RXM0EID8, 0x00);
	SPI_set(self, RXM0EID0, 0x00);

	// clear receve filters
	SPI_set(self, RXF0SIDL, 0x00);

	// enter config mode
	SPI_set(self, CANCTRL, REQOP_CONFIG);

	// configure bit rate and bit timing
	SPI_set(self, CNF2, 0x80 | PHSEG1_3TQ | PRSEG_1TQ);
	SPI_set(self, CNF3, PHSEG2_3TQ);

	// enable receive interrupt
	SPI_set(self, CANINTE, RX0IE);

	// clear transmit buffers
	SPI_set(self, TXB0CTRL, 0);

	// return to normal mode
	SPI_set(self, CANCTRL, REQOP_NORMAL | CLKOUT_ENABLED | OSM_ENABLED);

	// clear transmit buffers
	SPI_set(self, TXB0CTRL, 0);

	//Verify device entered Normal mode
	dummy = SPI_get(self, CANSTAT);
	if (OPMODE_NORMAL != (dummy && 0xE0))
		SPI_set(self, CANCTRL, REQOP_NORMAL | CLKOUT_ENABLED);
		// What good does it do doing this twice rather than in a while loop? why not OSM_EN again?

	// clear receive buffers
	SPI_set(self, RXB0CTRL, 0x60);
	SPI_set(self, RXB1CTRL, 0x60);
}
