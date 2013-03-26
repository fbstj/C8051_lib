/*
	MAX7301 GPIO port expansion
*/
#include "platform.h"
#include "MAX7301.h"

enum CMD {
	CONF = 0x04,
	TDM  = 0x06, // transition detect mask
#define PC(i)	( 8 + ((i) /  4))	// port i configuration register
#define PORT(i)	(32 + ((i) % 32))	// port i bit
#define P8(i)	(64 + ((i) % 32))	// port i thru i+8 word

	// Port configuration settings
	OUTPUT = 1, INPUT = 2, PULLUP = 3
};

static void IO_set(const struct MAX7301 * const self, const enum CMD command, const char value)
{
	self->word(((command & ~0x80) << 8) | value);
}
static char IO_get(const struct MAX7301 * const self, const enum CMD command)
{
	self->word((command | 0x80) << 8);
	return self->word(0x8000);
}

void MAX7301_init(const struct MAX7301 * const self)
{
	IO_set(self, CONF, 1);
}

void MAX7301_conf(const struct MAX7301 * const self, char p, char conf)
{
int shift = (p % 4) << 1,	mask = 3 << shift;
	IO_set(self, PC(p), (((conf & 3) << shift) & mask) | (IO_get(self, PC(p)) & ~mask));
}

char MAX7301_get(const struct MAX7301 * const self, char p)
{
	return IO_get(self, PORT(p));
}

void MAX7301_set(const struct MAX7301 * const self, char p, char v)
{
	IO_set(self, PORT(p), v & 1);
}

void MAX7301_change(const struct MAX7301 * const self, char p, char v, char m)
{	// changes p thru p+7 to (v & m), leaving ~m alone
	IO_set(self, P8(p % 32), (v & m) | (IO_get(self, P8(p % 32)) & ~m));
}

