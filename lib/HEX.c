/*
	Hexadecimal string parsing/writing functions
*/
#include "HEX.h"
#include <string.h>		// strcat, memcpy
#include <stdio.h>		// sprintf
#include <stdlib.h>		// strtoul

static char tmp[3];
static int i, j;
void HEX_parse(char * const out, const char * const in, const int len)
{
	for(i = j = 0; i < len; i++, j += 2)
	{
	    memcpy(tmp, in + j, 2);
	    out[i] = strtol(tmp, NULL, 16);
	}
}
void HEX_write(char * const out, const char * const in, const int len)
{
	out[0] = 0;	// clear out buffer
	for(i = 0; i < len; i++)
	{
		sprintf(tmp, "%02X", in[i]);
		strcat(out, tmp);
	}
}
