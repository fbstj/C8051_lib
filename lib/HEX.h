/*
	Hexadecimal string parsing/writing functions
*/
#ifndef __HEX_H
#define __HEX_H

// convert a hex string to an array of bytes
// -- parse(to, hex string, byte count)
extern void HEX_parse(char * const out, const char * const in, const int len);
// convert an array of bytes into a hex string
// -- write(to, bytes, byte count)
extern void HEX_write(char * const out, const char * const in, const int len);

#endif // __HEX_H