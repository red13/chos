#ifndef RING_BUFFER_H
#define RING_BUFFER_H

struct ring_buffer{
	unsigned long*	buf;
	unsigned long	size;
	unsigned long	wp;
	unsigned long	rp;
};
typedef struct ring_buffer ring_buffer_t;


/* prototypes */
void init_ring_buffer( ring_buffer_t* ring, int size, unsigned long* buf );
unsigned long writable_size_ring_buffer( ring_buffer_t* ring );
unsigned long readable_size_ring_buffer( ring_buffer_t* ring );
int write_ring_buffer(ring_buffer_t* ring, unsigned long val);
int read_ring_buffer(ring_buffer_t* ring, unsigned long* val);


#endif /* RING_BUFFER_H */
