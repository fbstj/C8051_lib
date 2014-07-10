/*
	library for communicating with EA DIP LCD screen
*/
#include "platform.h"
#include "EAeDIP.h"

enum chars {
	_DC1 = 0x11,
	_DC2 = 0x12,
	_ACK = 0x06,
	_ESC = 0x1B
};

#define LCD_timeout		1500

// protocol
static unsigned char check;

static char edip_ack(device_pt self)
{
char ack;
unsigned int timeout = 0;
	while ((ack != _ACK) && (timeout < LCD_timeout))
		if(self->pending())
			ack = self->read();
		else
			++timeout;
	return !(timeout < LCD_timeout && ack == _ACK);
}

static void edip_start(device_pt self, unsigned char length)
{
static char out[2];
	out[0] = _DC1;
	out[1] = length;
	self->send(out, 2);
	check = _DC1 + length;
}

static void edip_puts(device_pt self, char * buf, unsigned char length)
{
unsigned char i;
	for (i = 0; i < length; i++)
	{
		check += buf[i];
	}
	self->send(buf, length);
}

static char edip_finish(device_pt self)
{
	self->send(&check, 1);
	return edip_ack(self);
}

// helpers
#include <string.h>

static void edip_command(device_pt self, char cmd[2], unsigned char length)
{
static char out[4];
	edip_start(self, length + 3);
	out[0] = _ESC;
	out[1] = cmd[0];
	out[2] = cmd[1];
	out[3] = 0;
	edip_puts(self, out, 3);
}

void edip_u16(device_pt self, unsigned int value)
{
static char out[3];
	out[0] = value; out[1] = value >> 8; out[2] = 0;
	edip_puts(self, out, 2);
}

// macros
#define command(cmd, len, content)\
	edip_command(self, #cmd, len);\
	content;\
	edip_finish(self)
#define byte(ch)			edip_puts(self, &ch, 1)
#define word(word)			edip_u16(self, word)


// methods
void edip_clear(device_pt self)
{	// DL
	command(DL, 0, ;);
}

void edip_terminal(device_pt self, char on)
{	// TA or TE
	if (on == 0)
	{
		command(TA, 0, ;);
	}
	else
	{
		command(TE, 0, ;);
	}
}

void edip_font(device_pt self, unsigned char font)
{	// ZF<font>
	command(ZF,1, byte(font));
}

void edip_font_color(device_pt self, unsigned char fg, unsigned char bg)
{	// FZ<fg><bg>
	command(FZ, 2,
		byte(fg); byte(bg);
	);
}

void edip_font_zoom(device_pt self, unsigned char x, unsigned char y)
{	// ZZ<x><y>
	command(ZZ, 2,
		byte(x); byte(y);
	);
}

void edip_draw(device_pt self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{	// GR<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi>
	command(GR, 8,
		word(x1);	word(y1);
		word(x2);	word(y2);
	);
}

void edip_fill(device_pt self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color)
{	// RF<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi><color>
	command(RF, 9,
		word(x1);	word(y1);
		word(x2);	word(y2);
		byte(color);
	);
}

void edip_delete(device_pt self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{	// RL<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi>
	command(RL, 8,
		word(x1);	word(y1);
		word(x2);	word(y2);
	);
}

void edip_text(device_pt self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char pos, char *str)
{	// ZB<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi><pos><string...>\0
unsigned char length = strlen(str);
	command(ZB, 10 + length,
		word(x1);	word(y1);
		word(x2);	word(y2);
		byte(pos);
		edip_puts(self, str, length);
		length = 0;	byte(length);
	);
}

void edip_clear_keys(device_pt self)
{	// AL\0\1
	command(AL, 2,
		edip_puts(self, "\0\1", 2);
	);
}

void edip_button(device_pt self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char down, unsigned char up, char *str)
{	// AT<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi><down><up><string...>\0
unsigned char length = strlen(str);
	command(AT, 12 + length,
		word(x1);	word(y1);
		word(x2);	word(y2);
		byte(down);	byte(up);
		edip_puts(self, str, length);
		length = 0;	byte(length);
	);
}

void edip_switch(device_pt self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char down, unsigned char up, char *str)
{	// AK<x1_lo><x1_hi><y1_lo><y1_hi><x2_lo><x2_hi><y2_lo><y2_hi><down><up><string...>\0
unsigned char length = strlen(str);
	command(AK, 12 + length,
		word(x1);	word(y1);
		word(x2);	word(y2);
		byte(down);	byte(up);
		edip_puts(self, str, length);
		length = 0;	byte(length);
	);
}

void edip_brightness(device_pt self, unsigned char brightness)
{	// YH<brightness>
	command(YH, 1, byte(brightness));
}
