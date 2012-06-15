/*
	A library for building pseudo-NMEA packets
*/
#include "NMEA.h"
#include <ctype.h>		// isprint
#include <string.h>		// strlen
#include <stdio.h>		// sprintf
#include <stdlib.h>		// strtoul

// return the calculated checksum of msg
unsigned char NMEA_checksum(NMEA_msg_t *const msg)
{
unsigned char i;
	msg->State = e_NMEA_STAR;
	msg->Checksum = 0xFF;
	for (i = 0; i < msg->Length; i++)
		msg->Checksum ^= msg->String[i];
	return msg->Checksum;
}

void NMEA_void(NMEA_msg_t *const msg)
{
	memset(msg, 0, sizeof(NMEA_msg_t));
	msg->State = e_NMEA_INVALID;
}

unsigned char NMEA_argc(NMEA_msg_t *const msg)
{
unsigned char i;
	msg->Arguments = 0;
	for (i = 0; i < msg->Length; i++)
		msg->Arguments += msg->String[i] == e_NMEA_DELIMIT;
	return msg->Arguments;
}

char NMEA_start(NMEA_msg_t *const msg, const char *const command)
{
unsigned char l = strlen(command);
	msg->State = e_NMEA_DOLLAR;
	msg->Arguments = 0;
	for (msg->Length = 0; msg->Length < l; msg->Length++)
		msg->String[msg->Length] = command[msg->Length];
	return msg->Length;
}

char NMEA_add(NMEA_msg_t *const msg, const char *const arg)
{
unsigned char i, l = strlen(arg);
	msg->String[msg->Length++] = e_NMEA_DELIMIT;
	for (i = 0; i < l; i++)
		msg->String[msg->Length + i] = arg[i];
	msg->Length += i;
	msg->String[msg->Length] = 0;
	msg->Arguments++;
	return l;
}

char NMEA_finish(NMEA_msg_t *const msg, char *const out)
{
unsigned char i;
	out[0] = e_NMEA_DOLLAR;
	for (i = 0; i < msg->Length; i++)
		out[1 + i] = msg->String[i];
	NMEA_checksum(msg);
	out[i + 1] = msg->State;
	sprintf(out + i + 2,"%02X", msg->Checksum);
	msg->State = e_NMEA_VALID;
	return i + 4;
}

unsigned char NMEA_get(NMEA_msg_t *const msg, unsigned char index, char *const buf)
{
unsigned char i = 0, j = 0;
	if (msg->State != e_NMEA_VALID || msg->Arguments < index)
		return 0;
	// navigate to index'th argument
	while (index > 0)
		if(msg->String[i++] == e_NMEA_DELIMIT)
			index--;
	while (msg->String[i] != e_NMEA_DELIMIT && isprint(msg->String[i]))
		buf[j++] = msg->String[i++];
	buf[j] = 0;
	return j;
}

char NMEA_parse_byte(NMEA_msg_t *const msg, const unsigned char byte)
{
static char STARS, STAR[2];
	if (byte == e_NMEA_DOLLAR)
	{
		msg->Length = 0;
		return msg->State = byte;
	}
	if (!isprint(byte))
		return msg->State = e_NMEA_INVALID;
	if (byte == e_NMEA_STAR)
	{
		STARS = 0;
		return msg->State = byte;
	}
	switch (msg->State)
	{
	case e_NMEA_DOLLAR:
		msg->String[msg->Length++] = byte;
		break;

	case e_NMEA_STAR:
		STAR[STARS++] = byte;
		if(STARS < 2)
			return msg->State;
		if(NMEA_checksum(msg) != strtoul(STAR, NULL, 16))
			return msg->State == e_NMEA_INVALID;
		NMEA_argc(msg);
		msg->State = e_NMEA_VALID;
		break;
	}
}

char NMEA_parse_string(NMEA_msg_t *const msg, const char *const str)
{
unsigned char i = 0, l = strlen(str);
	for(i = 0; i < l; i++)
	{
		NMEA_parse_byte(msg, str[i]);
		if(msg->State == e_NMEA_VALID)
			return i + 1;
	}
	return 0;
}

