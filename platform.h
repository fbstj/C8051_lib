/*
	platform dependent definitions and macros
*/
#ifndef __PLATFORM_H
#define __PLATFORM_H
#include "compiler_defs.h"		// compiler independant definitions
#include "C8051F580_defs.h"		// target dependant definitions

#define SYSCLK	24000000		// SYSCLK is 24MHz

extern Init_Device();			// the Config Wizard entry point

struct device {
	void (*init)();
	int (*pending)();
	int (*read)();
	int (*sending)();
	int (*send)();
	void (*isr)();
	void * etc;
};
typedef const struct device * device_pt;

#endif	// __PLATFORM_H
