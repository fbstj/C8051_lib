/*
	MAX7301 GPIO port expansion
*/
#ifndef __MAX7301_H
#define __MAX7301_H

struct MAX7301 {
	// exchange a word with the device (including chip selection)
	char (*command)(char, char);
};

#define MAX7301_new(name, cs, byte)\
	static char name##_word(char cmd, char value) { cs = 0; byte(cmd); value = byte(value); cs = 1; return value; }\
	code const struct MAX7301 name = { name##_word }

// start the MAX7301  device
extern void MAX7301_init(const struct MAX7301 * const);
// configure a port pin
// -- conf(device, port, configuration{1:output,2:input,3:pullup})
extern void MAX7301_conf(const struct MAX7301 * const, char, char conf);
// get the state of a port pin
extern char MAX7301_get(const struct MAX7301 * const, char);
// set the state of a port pin
extern void MAX7301_set(const struct MAX7301 * const, char, char value);
// chnange up to 8 port pins
// - changes port thru port+7 to (value & mask) leaving (~mask) unchanged
extern void MAX7301_change(const struct MAX7301 * const, char, char value, char mask);

#endif // __MAX7301_H