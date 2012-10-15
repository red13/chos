/**
 * @file
 */

#include "ring_buffer.h"


/**
 * @brief
 */
void init_ring_buffer( ring_buffer_t* ring, int size, unsigned long* buf )
{
	ring->buf  = buf;
	ring->size = size;
	ring->wp   = 0;
	ring->rp   = size-1;
}

unsigned long writable_size_ring_buffer( ring_buffer_t* ring )
{
	unsigned long ws = 0;

	if( ring )
	{
		if( ring->wp > ring->rp )
		{
			/* 0, rp, wp, size */
			ws = ring->size - ring->wp + ring->rp;
		}
		else
		{
			/* 0, wp, rp, size */
			ws = ring->rp - ring->wp;
		}
	}
	return ws;
}

unsigned long readable_size_ring_buffer( ring_buffer_t* ring )
{
	unsigned long rs = 0;

	if( ring )
	{
		rs = ring->size - writable_size_ring_buffer( ring );
	}

	return rs;
}


int read_ring_buffer(ring_buffer_t* ring, unsigned long* val)
{
	int ret = -1;
	if( val )
	{
		if( 0 < readable_size_ring_buffer(ring) )
		{
			*val = ring->buf[ring->rp];
			ring->rp = (ring->rp + 1) % ring->size;
			ret = 0;
		}
	}
	return ret;
}

int write_ring_buffer(ring_buffer_t* ring, unsigned long val)
{
	int ret = -1;
	if( 0 < writable_size_ring_buffer(ring) )
	{
		ring->buf[ring->wp] = val;
		ring->wp = (ring->wp + 1) % ring->size;
		ret = 0;
	}
	return ret;
}

