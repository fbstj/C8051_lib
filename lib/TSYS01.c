/*
	TSYS01 SPI temperature sensor
*/
#include "platform.h"
#include "TSYS01.h"

void TSYS01_init(struct TSYS01 * self)
{	// reset the chip
static char i;
static unsigned int v;
	self->select();
	self->byte(0x1E);

	while (self->pin() == 0) ;

	self->deselect();

	for (i = 0; i < 8; i++)
	{
		self->select();
		self->byte(0xA0 + ((i) << 1));
		v = self->byte(0);
		v = self->byte(0) | (v << 8);
		self->regs[i] = v;
		self->deselect();
	}
}

long TSYS01_read(struct TSYS01 * self){
static long value;
	self->select();
	self->byte(0x48) == 0xFE;
	while (self->pin() == 0);
	self->deselect();
	// gap?
	self->select();
	self->byte(0x00) == 0xFE;
	value = self->byte(0);
	value = self->byte(0) | (value << 8);
	value = self->byte(0) | (value << 8);
	self->deselect();
	return value;
}

float TSYS01_poly(struct TSYS01 *self, int x)
{
static float y;
	y = 		-2e-21 * self->regs[1];
	y = y * x + 4e-16 * self->regs[2];
	y = y * x - 2e-11 * self->regs[3];
	y = y * x + 1e-6 * self->regs[4];
	y = y * x - 15e-3 * self->regs[5];
	return y;
}
