/*
	Extra useful functions for the NMEA library
*/
#include "NMEA.h"
#include <ctype.h>		// isprint
#include <string.h>		// strlen
#include <stdio.h>		// sprintf
#include <stdlib.h>		// strtoul

unsigned char buf[20];

long NMEA_get_number(struct NMEA_msg * const msg, const int index, const int base)
{
	NMEA_get(msg, index, buf);
	return strtoul(buf, NULL, base);
}

char NMEA_add_number(struct NMEA_msg * const msg, const long number, char * const format)
{
	sprintf(buf, format, number);
	return NMEA_add(msg, buf);
}

char NMEA_arg_equal(struct NMEA_msg * const msg, const int index, char * const string)
{
	NMEA_get(msg, index, buf);
	return strcmp(buf,string) == 0;
}

char NMEA_parse_string(struct NMEA_msg * const msg, const char *const str)
{
unsigned char i = 0, l = strlen(str);
	for(i = 0; i < l; i++)
	{
		NMEA_parse_byte(msg, str[i]);
		if(msg->state == NMEA_valid)
			return i + 1;
	}
	return 0;
}

void NMEA_clear(struct NMEA_msg * const msg)
{
	memset(msg, 0, sizeof(struct NMEA_msg));
	msg->state = NMEA_invalid;
}
