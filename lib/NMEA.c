/*
	A library for building pseudo-NMEA packets
*/
#include "NMEA.h"
#include <ctype.h>		// isprint
#include <string.h>		// strlen
#include <stdio.h>		// sprintf
#include <stdlib.h>		// strtoul

// return the calculated checksum of msg
unsigned char NMEA_checksum(struct NMEA_msg * const msg)
{
unsigned char i;
	msg->state = NMEA_star;
	msg->check = 0xFF;
	for (i = 0; i < msg->length; i++)
		msg->check ^= msg->raw[i];
	return msg->check;
}

int NMEA_argc(struct NMEA_msg * const msg)
{
int i;
	msg->argc = 0;
	for (i = 0; i < msg->length; i++)
		msg->argc += msg->raw[i] == NMEA_delimit;
	return msg->argc;
}

char NMEA_start(struct NMEA_msg * const msg, const char *const command)
{
unsigned char l = strlen(command);
	msg->state = NMEA_dollar;
	msg->argc = 0;
	for (msg->length = 0; msg->length < l; msg->length++)
		msg->raw[msg->length] = command[msg->length];
	return msg->length;
}

char NMEA_add(struct NMEA_msg * const msg, const char *const arg)
{
unsigned char i, l = strlen(arg);
	msg->raw[msg->length++] = NMEA_delimit;
	for (i = 0; i < l; i++)
		msg->raw[msg->length + i] = arg[i];
	msg->length += i;
	msg->raw[msg->length] = 0;
	msg->argc++;
	return l;
}

char NMEA_finish(struct NMEA_msg * const msg, char *const out)
{
unsigned char i;
	out[0] = NMEA_dollar;
	for (i = 0; i < msg->length; i++)
		out[1 + i] = msg->raw[i];
	NMEA_checksum(msg);
	out[i + 1] = msg->state;
	sprintf(out + i + 2,"%02X", msg->check);
	msg->state = NMEA_valid;
	return i + 4;
}

int NMEA_get(struct NMEA_msg * const msg, int index, char *const buf)
{
unsigned char i = 0, j = 0;
	if (msg->state != NMEA_valid || msg->argc < index)
		return 0;
	// navigate to index'th argument
	while (index > 0)
		if(msg->raw[i++] == NMEA_delimit)
			index--;
	while (msg->raw[i] != NMEA_delimit && isprint(msg->raw[i]))
		buf[j++] = msg->raw[i++];
	buf[j] = 0;
	return j;
}

enum NMEA_state NMEA_parse_byte(struct NMEA_msg * const msg, const unsigned char byte)
{
static char STARS, STAR[3];
	if (byte == NMEA_dollar)
		return (msg->length = 0, msg->state = byte);
	if (!isprint(byte))
		return msg->state = NMEA_invalid;
	if (byte == NMEA_star)
		return (STARS = 0, msg->state = byte);
	switch (msg->state)
	{
	case NMEA_dollar:
		msg->raw[msg->length++] = byte;
		break;

	case NMEA_star:
		STAR[STARS++] = byte;
		if(STARS < 2)
			return msg->state;
		STAR[2] = 0;
		if(NMEA_checksum(msg) != strtoul(STAR, NULL, 16))
			return msg->state = NMEA_invalid;
		NMEA_argc(msg);
		msg->state = NMEA_valid;
		break;
	}
	return msg->state;
}

