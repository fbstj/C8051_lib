/*
	platform dependent definitions and macros
*/
#ifndef __PLATFORM_H
#define __PLATFORM_H
#include "compiler_defs.h"		// compiler independant definitions
#include "C8051F580_defs.h"		// target dependant definitions

#define SYSCLK	48000000		// SYSCLK is 48MHz

extern Init_Device();			// the Config Wizard entry point

#endif	// __PLATFORM_H
