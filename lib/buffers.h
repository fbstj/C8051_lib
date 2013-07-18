/*
	static memory buffers
*/
#ifndef __BUFFERS_H
#define __BUFFERS_H

// ring maths helpers
#define ring_count(len, head, tail)	(((len) + (head) - (tail)) % (len))
#define ring_space(len, head, tail)	((len) - ring_count(len, head, tail))

enum RING_STATE { RING_OK = 0, RING_EMPTY, RING_FULL, RING_ERROR };

// typeless ring, with helper functions
struct RING { void * raw; unsigned int length, width, offset, count; };
#define RING_head(r)	(((r)->offset + (r)->count) % (r)->length)
enum RING_STATE RING_add(struct RING * const, const void * const);
enum RING_STATE RING_get(void * const, struct RING * const);

#define RING_clear(r)	(r)->head = (r)->tail = 0
struct byte_ring { unsigned char buffer[256], head, tail; };

#endif // __BUFFERS_H

