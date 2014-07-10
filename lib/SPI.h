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
};
typedef const struct spi_device * spi_pt;

#endif // __SPI_H
