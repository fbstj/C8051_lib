/*
	code for calculating various cyclic redundancy checks
	based on code by Ross Williams (ross@guest.adelaide.edu.au.) in the public domain
*/
#ifndef __CRC_H
#define __CRC_H

typedef struct T_CRC_MODEL {
	// polynomial width [8, 32]
	int Width;
	// polynomial value
	unsigned long Polynomial;
	// initial value of Value
	unsigned long Initial;
	// Reflections flags: bit-0 reflects input, bit-1 reflects output
	char Reflect;
	// current value of the CRC
	unsigned long XOR_output;

	// current value of the CRC
	unsigned long Value;
}	CRC_model_t;

// start checking a new bit-string
void CRC_init(CRC_model_t *model);

// add 8 bits from the bit-string to the model
void CRC_next(CRC_model_t *model, unsigned char next);

// returns the CRC value for the bit-string processed so far
unsigned long CRC_value(CRC_model_t *model);

#endif	// __CRC_H