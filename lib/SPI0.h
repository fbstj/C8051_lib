/*
	SPI0 peripheral driver for C8051F580
*/

// write a byte to SPI0 and return the latest byte received
extern unsigned char SPI0_byte(unsigned char const write);

// a device on the SPI bus (device 0 reseved to unselect)
typedef enum E_SPI0_DEVICES SPI0_device;

// select the device passed
extern void SPI0_select(SPI0_device device);
// select the device passed
extern SPI0_device SPI0_selected(void);
