/*
	TSYS01 SPI temperature sensor
*/
#include "platform.h"
#include "TSYS01.h"

#define SELECT(action) self->select(); action; self->deselect()

void TSYS01_init(spi_pt self, unsigned char regs[8])
{	// reset the chip
static char i;
static unsigned int v;
	SELECT( self->byte(0x1E); while (self->isr() == 0) ; );

	for (i = 0; i < 8; i++)
	{
		SELECT(
			self->byte(0xA0 + ((i) << 1));
			v = self->byte(0);
			v = self->byte(0) | (v << 8);
			regs[i] = v;
		);
	}
	self->etc = regs;
}

long TSYS01_read(spi_pt self){
static long value;
	SELECT( self->byte(0x48) == 0xFE; while (self->isr() == 0) ; );
	// gap?
	SELECT(
		self->byte(0x00) == 0xFE;
		value = self->byte(0);
		value = self->byte(0) | (value << 8);
		value = self->byte(0) | (value << 8);
	);
	return value;
}

float TSYS01_poly(spi_pt self, int x)
{
static float y;
	y = 		-2e-21 * self->etc[1];
	y = y * x + 4e-16 * self->etc[2];
	y = y * x - 2e-11 * self->etc[3];
	y = y * x + 1e-6 * self->etc[4];
	y = y * x - 15e-3 * self->etc[5];
	return y;
}
