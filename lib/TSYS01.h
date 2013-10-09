/*
	TSYS01 SPI temperature sensor
*/
#ifndef __TSYS01_H
#define __TSYS01_H

struct TSYS01 {
	unsigned char (*byte)(unsigned char);
	void (*select)();
	void (*deselect)();
	char (*pin)();
	unsigned int regs[8];
};

extern void TSYS01_init(struct TSYS01 *);

extern long TSYS01_read(struct TSYS01 *);

extern float TSYS01_poly(struct TSYS01 *, int);

#endif	// __TSYS01_h