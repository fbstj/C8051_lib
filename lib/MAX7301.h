/*
	MAX7301 GPIO port expansion
*/
#ifndef __MAX7301_H
#define __MAX7301_H
#include "SPI.h"

// start the MAX7301  device
extern void MAX7301_init(spi_pt);
// configure a port pin
// -- conf(device, port, configuration{1:output,2:input,3:pullup})
extern void MAX7301_conf(spi_pt, char, char conf);
// get the state of a port pin
extern char MAX7301_get(spi_pt, char);
// set the state of a port pin
extern void MAX7301_set(spi_pt, char, char value);
// chnange up to 8 port pins
// - changes port thru port+7 to (value & mask) leaving (~mask) unchanged
extern void MAX7301_change(spi_pt, char, char value, char mask);

#endif // __MAX7301_H