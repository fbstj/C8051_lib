/*
	A library for building pseudo-NMEA packets
*/

/* 	--- constants and types --- */
#define NMEA_BUF_LEN	256

enum E_NMEA_STATES {
	e_NMEA_DOLLAR = '$',
	e_NMEA_STAR = '*',
	e_NMEA_DELIMIT = ',',
	e_NMEA_INVALID = 0,
	e_NMEA_VALID = 1
};

typedef struct NMEA_MESSAGE_T {
	unsigned char String[NMEA_BUF_LEN];
	unsigned char Length;		// nunber of characters in String
	unsigned char Arguments;	// number of arguments in String
	unsigned char Checksum;		// current CRC of String
	enum E_NMEA_STATES State;	// current state of parsing
} NMEA_msg_t;

/* 	--- functions --- */
// parse a byte into msg
extern char NMEA_parse_byte(NMEA_msg_t *const msg, const unsigned char byte);

// parse a string into a mmsg
extern char NMEA_parse_string(NMEA_msg_t *const msg, const char *const str);

// start constructing a msg
extern char NMEA_start(NMEA_msg_t *const msg, const char *const command);

// add an argument to msg
extern char NMEA_add(NMEA_msg_t *const msg, const char *const arg);

// copy and complete msg into out
extern char NMEA_finish(NMEA_msg_t *const msg, char *const out);

// return the calculated checksum of msg (a bytewise XOR between $ and *, starting at 0xFF)
extern unsigned char NMEA_checksum(NMEA_msg_t *const msg);
// return the calculated number of arguments in msg
extern unsigned char NMEA_argc(NMEA_msg_t *const msg);

// copy the index'th argument from str into buf, return the length of the argument
extern unsigned char NMEA_get(NMEA_msg_t *const msg, unsigned char index, char *const buf);
// retreive the command string from msg into buf
#define NMEA_command(msg, buf)		NMEA_get(msg, 0, buf)

// -- EXTRA (NMEA_extra.c)
// clear the passed message
extern void NMEA_void(NMEA_msg_t *const msg);
// return the numeric value of the index'th argument in msg, converting it using the specified base
extern unsigned long NMEA_get_number(NMEA_msg_t *const msg, const unsigned char index, const unsigned char base);
// add the passed number using the passed string format
extern char NMEA_add_number(NMEA_msg_t *const msg, const unsigned long number, unsigned char *const format);
// asserts the equality of the index'th argument in msg and the passed string
extern char NMEA_arg_equal(NMEA_msg_t *const msg, const unsigned long index, unsigned char *const string);

// -- end of file

