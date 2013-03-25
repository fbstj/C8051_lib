/*
	static memory buffers
*/
#ifndef __BUFFERS_H
#define __BUFFERS_H

// ring maths helpers
#define ring_count(len, head, tail)	(((len) + (head) - (tail)) % (len))
#define ring_space(len, head, tail)	((len) - ring_count(len, head, tail))

// typeless rings, with helper functions
//#define T_RING(T, tag, len) struct tag { T buffer[len]; unsigned int head, tail; }
#define RING_length(o)	(sizeof((o)->buffer) / sizeof((o)->buffer[0]))
#define RING_count(o)	ring_count(RING_length(o), (o)->head, (o)->tail)
#define RING_space(o)	ring_space(RING_length(o), (o)->head, (o)->tail)
#define RING_clear(o)	(o)->head = (o)->tail = 0

#define RING_put(o)		(o)->buffer[(o)->head++]
#define RING_get(o)		(o)->buffer[(o)->tail++]

struct byte_ring { unsigned char buffer[256], head, tail; };

#endif // __BUFFERS_H

