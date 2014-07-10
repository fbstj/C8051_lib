/*
	spi device structure and spi_pt
*/
#ifndef __SPI_H
#define __SPI_H

struct spi_device {
	void (*init)();
	void (*select)();
	void (*deselect)();
	unsigned char (*byte)(unsigned char);
	char (*isr)();
	char * etc;
};
typedef const struct spi_device * spi_pt;

#define SPI_device(name, init, enter, exit, isr, byte)\
	static void name##_sel() enter \
	static void name##_des() exit \
	static void name##_isr() isr \
	const struct spi_device name = { init, name##_sel, name##_des, byte, isr }

#endif // __SPI_H
