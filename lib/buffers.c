/*
	static memory buffers
*/
#include "buffers.h"
#include <string.h>

enum RING_STATE RING_add(struct RING * const ring, const void * const item)
{
	if (ring->offset > ring->length || ring->count > ring->length)
		return RING_ERROR;
	if (ring->count == ring->length)
		return RING_FULL;
	memcpy(ring->raw + RING_head(ring) * ring->width, item, ring->width);
	ring->count++;
	return RING_OK;
}

enum RING_STATE RING_get(void * const item, struct RING * const ring)
{
	if (ring->offset > ring->length || ring->count > ring->length)
		return RING_ERROR;
	if (ring->count == 0)
		return RING_EMPTY;
	memcpy(item, ring->raw + ring->offset * ring->width, ring->width);
	ring->offset = (ring->offset + 1 ) % ring->length;
	ring->count--;
	return RING_OK;
}

