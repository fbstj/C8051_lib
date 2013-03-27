/*
	A library for building pseudo-NMEA packets
*/

/* 	--- constants and types --- */
#define NMEA_BUF_LEN	256

enum NMEA_state {
	NMEA_dollar = '$',
	NMEA_star = '*',
	NMEA_delimit = ',',
	NMEA_invalid = 0,
	NMEA_valid = 1
};

struct NMEA_msg {
	unsigned char raw[NMEA_BUF_LEN];
	unsigned char length;	// nunber of characters in String
	unsigned char argc;		// number of arguments in String
	unsigned char check;	// current CRC of String
	enum NMEA_state state;	// current state of parsing
};

/* 	--- functions --- */
// parse a byte into msg
extern enum NMEA_state NMEA_parse_byte(struct NMEA_msg * const, const char);

// start constructing a msg
extern char NMEA_start(struct NMEA_msg * const, const char * const);

// add an argument to msg
extern char NMEA_add(struct NMEA_msg * const, const char * const);

// copy and complete msg into out
extern char NMEA_finish(struct NMEA_msg * const, char * const out);

// return the calculated checksum of msg (a bytewise XOR between $ and *, starting at 0xFF)
extern unsigned char NMEA_checksum(struct NMEA_msg * const);

// return the calculated number of arguments in msg
// argc(msg)
extern int NMEA_argc(struct NMEA_msg * const);

// copy the index'th argument from str into buf, return the length of the argument
// get(msg, index, buffer)
extern int NMEA_get(struct NMEA_msg * const, const int, char * const);
// retreive the command string from msg into buf
#define NMEA_command(msg, buf)		NMEA_get(msg, 0, buf)

// -- EXTRA (NMEA_extra.c)
// clear the passed message
extern void NMEA_clear(struct NMEA_msg * const);

// return the numeric value of the index'th argument in msg, converting it using the specified base
// - get(msg, index, base)
extern long NMEA_get_number(struct NMEA_msg * const, const int, const int);

// add the passed number using the passed string format
// - add(msg, number, format)
extern char NMEA_add_number(struct NMEA_msg * const, const long, char * const);

// asserts the equality of the index'th argument in msg and the passed string
// - equal(msg, index, cmp)
extern char NMEA_arg_equal(struct NMEA_msg * const, const int, char * const);

// parse a string into a mmsg
extern char NMEA_parse_string(struct NMEA_msg * const, const char * const);

// -- end of file

