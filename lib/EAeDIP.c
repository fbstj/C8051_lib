/*
	library for communicating with EA DIP LCD screen
*/
#include "EAeDIP.h"

enum chars {
	DC1 = 0x11,
	DC2 = 0x12,
	ACK = 0x06,
	ESC = 0x1B
};

typedef const struct EDIP * self_t;
#define LCD_timeout		5000
#define LCD_resetSleep	100000

// protocol
static unsigned char check;

static void sleep(volatile unsigned long x){ while (x--) ; }

static char edip_ack(self_t self)
{
char ack;
unsigned int timeout = 0;
	while ((ack != ACK) && (timeout < LCD_timeout))
		if(self->pending())
			ack = self->getc();
		else
			++timeout;
	sleep(1000);
	return !(timeout < LCD_timeout && ack == ACK);
}

static void edip_start(self_t self, unsigned char length)
{
static char out[2];
	out[0] = DC1;
	out[1] = length;
	self->puts(out, 2);
	check = DC1 + length;
}

static void edip_puts(self_t self, char * buf, unsigned char length)
{
unsigned char i;
	for (i = 0; i < length; i++)
	{
		check += buf[i];
	}
	self->puts(buf, length);
}

static char edip_finish(self_t self)
{
	self->puts(&check, 1);
	return edip_ack(self);
}

// helpers
#include <string.h>

static void edip_command(self_t self, char cmd[2], unsigned char length)
{
static char out[4];
	edip_start(self, length + 3);
	out[0] = ESC;
	out[1] = cmd[0];
	out[2] = cmd[1];
	out[3] = 0;
	edip_puts(self, out, 3);
}

void edip_u16(self_t self, unsigned int value)
{
static char out[3];
	out[0] = value; out[1] = value >> 8; out[2] = 0;
	edip_puts(self, out, 2);
}

// macros
#define command(cmd, len)	edip_command(self, #cmd, len)
#define byte(ch)			edip_puts(self, &ch, 1)
#define word(word)			edip_u16(self, word)
#define done()				edip_finish(self)

// methods
void edip_clear(self_t self)
{	// DL
	command(DL, 0);
	done();
}

void edip_font(self_t self, unsigned char font)
{	// ZF<font>
	command(ZF,1);
	byte(font);
	done();
}

void edip_font_color(self_t self, unsigned char fg, unsigned char bg)
{	// FZ<fg><bg>
	command(FZ, 2);
	byte(fg); byte(bg);
	done();
}

void edip_font_zoom(self_t self, unsigned char x, unsigned char y)
{	// ZZ<x><y>
	command(ZZ, 2);
	byte(x); byte(y);
	done();
}

void edip_draw(self_t self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{	// GR<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi>
	command(GR, 8);
	word(x1);	word(y1);
	word(x2);	word(y2);
	done();
}

void edip_fill(self_t self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color)
{	// RF<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi><color>
	command(RF, 9);
	word(x1);	word(y1);
	word(x2);	word(y2);
	byte(color);
	done();
}

void edip_text(self_t self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char pos, char *str)
{	// ZB<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi><pos><string...>\0
unsigned char length = strlen(str);
	command(ZB, 10 + length);
	word(x1);	word(y1);
	word(x2);	word(y2);
	byte(pos);
	edip_puts(self, str, length);
	length = '\n';	byte(length);
	done();
}

void edip_clear_keys(self_t self)
{	// AL\0\1
	command(AL, 2);
	edip_puts(self, "\0\1", 2);
	done();
}

void edip_button(self_t self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char down, unsigned char up, char *str)
{	// AT<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi><down><up><string...>\0
unsigned char length = strlen(str);
	command(AT, 12 + length);
	word(x1);	word(y1);
	word(x2);	word(y2);
	byte(down);	byte(up);
	edip_puts(self, str, length);
	length = 0;	byte(length);
	done();
}

void edip_switch(self_t self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char down, unsigned char up, char *str)
{	// AK<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi><down><up><string...>\0
unsigned char length = strlen(str);
	command(AK, 12 + length);
	word(x1);	word(y1);
	word(x2);	word(y2);
	byte(down);	byte(up);
	edip_puts(self, str, length);
	length = 0;	byte(length);
	done();
}
