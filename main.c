/*
	Main initialisation and logic loop
*/
#include "platform.h"	// include platform specific definitions

// main function state

void main(void)
{
// initlaise
	PCA0MD &= ~0x40;	// disable watchdog
	Init_Device();		// device setup, init.c
	// init drivers

	// init state

	EA = 1;				// enable interrupts
// main loop
	while(1)
	{
	}
}
