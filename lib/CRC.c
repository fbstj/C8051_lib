/*
	code for calculating various cyclic redundancy checks
	based on code by Ross Williams (ross@guest.adelaide.edu.au.) in the public domain
*/
#include "CRC.h"

// -- internal functions
#define BITMASK(X) (1L << (X))
#define CRC_mask(width) ((((1L << (width - 1)) - 1L) << 1) | 1L)

// CRC_reflect(value, size) -> eulav
unsigned long CRC_reflect(unsigned long v, int b)
{
int i; unsigned long t = v;
	for (i = 0; i < b; i++)
	{
		if (t & 1L)
			v |= BITMASK((b - 1) - i);
		else
			v &= ~BITMASK((b - 1) - i);
		t >>= 1;
	}
	return v;
}

void CRC_init(CRC_model_t *model) { model->Value = model->Initial; }

void CRC_next(CRC_model_t *model, unsigned char next)
{
int i;
unsigned long uch = next;
unsigned long topbit = BITMASK(model->Width - 1);

	if (model->Reflect & 1 == 1)
	{	// reflect input
		uch = CRC_reflect(uch, 8);
	}
	// combine value and next
	model->Value ^= uch << (model->Width - 8);

	for (i = 0; i < 8; i++)
	{	// for each bit in `next`
		if (model->Value & topbit)
		{
			model->Value = (model->Value << 1) ^ model->Polynomial;
		}
		else
		{
			model->Value = model->Value << 1;
		}

		model->Value &= CRC_mask(model->Width);
	}
}

unsigned long CRC_value(CRC_model_t *model)
{
	if ((model->Reflect & 2) == 2)
	{
		return model->XOR_output ^ CRC_reflect(model->Value, model->Width);
	}
	else
	{
		return model->XOR_output ^ model->Value;
	}
}
