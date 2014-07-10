/*
	TSYS01 SPI temperature sensor
*/
#ifndef __TSYS01_H
#define __TSYS01_H
#include "SPI.h"

extern void TSYS01_init(spi_pt, unsigned char[]);

extern long TSYS01_read(spi_pt);

extern float TSYS01_poly(spi_pt, int);

#endif	// __TSYS01_h