/*
	MAX7301 GPIO port expansion
*/
#include "MAX7301.h"

enum CMD {
	CONF = 0x04,
	TDM  = 0x06, // transition detect mask
#define PC(i)	(8 + ((i) / 4))
#define PCs(i)	(((i) % 4) << 1)
#define PORT(i)	(0x20+(i))	// set port i's value in bit 0
#define P8(i)	(0x40+(i))	// set ports i thru i+8	to bits passed
	// Port configuration settings
	OUTPUT = 1, INPUT = 2, PULLUP = 3
};

#define IO_set	self->command
#define IO_get(cmd)	(self->command(cmd | 0x80, 0), self->command(0x80, 0))

#define IO_change(cmd, value, mask)	IO_set(cmd, ((value) & (mask)) | (IO_get(cmd) & ~(mask)))

void MAX7301_init(const struct MAX7301 * const self)
{
	IO_set(CONF, 1);
}

void MAX7301_conf(const struct MAX7301 * const self, char p, char conf)
{
char cmd = PC(p), shift = PCs(p), mask = 3 << shift;
	shift = (conf & 3) << PCs(p);	// because `<< shift` does not work?
	IO_change(cmd, shift, mask);
}

char MAX7301_get(const struct MAX7301 * const self, char p)
{
	return IO_get(PORT(p));
}

void MAX7301_set(const struct MAX7301 * const self, char p, char v)
{
	IO_set(PORT(p), v & 1);
}

void MAX7301_change(const struct MAX7301 * const self, char p, char v, char m)
{
	IO_change(P8(p % 32), v, m);
}
