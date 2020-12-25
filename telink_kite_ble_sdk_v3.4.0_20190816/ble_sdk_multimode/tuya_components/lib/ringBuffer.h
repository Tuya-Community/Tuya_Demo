
/*
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

/* ring buffer */
struct ringbuffer
{
    unsigned char *buffer_ptr;
    /* use the msb of the {read,write}_index as mirror bit. You can see this as
     * if the buffer adds a virtual mirror and the pointers point either to the
     * normal or to the mirrored buffer. If the write_index has the same value
     * with the read_index, but in a different mirror, the buffer is full.
     * While if the write_index and the read_index are the same and within the
     * same mirror, the buffer is empty. The ASCII art of the ringbuffer is:
     *
     *          mirror = 0                    mirror = 1
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Full
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     *  read_idx-^                   write_idx-^
     *
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Empty
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * read_idx-^ ^-write_idx
     *
     * The tradeoff is we could only use 32KiB of buffer for 16 bit of index.
     * But it should be enough for most of the cases.
     *
     * Ref: http://en.wikipedia.org/wiki/Circular_buffer#Mirroring */
    unsigned short read_mirror : 1;
    unsigned short read_index : 15;
    unsigned short write_mirror : 1;
    unsigned short write_index : 15;
    /* as we use msb of index as mirror bit, the size should be signed and
     * could only be positive. */
    short buffer_size;
};

enum ringbuffer_state
{
    RINGBUFFER_EMPTY,
    RINGBUFFER_FULL,
    /* half full is neither full nor empty */
    RINGBUFFER_HALFFULL,
};

/**
 * RingBuffer for DeviceDriver
 *
 * Please note that the ring buffer implementation of RT-Thread
 * has no thread wait or resume feature.
 */
void ringbuffer_init(struct ringbuffer *rb, unsigned char *pool, short size);
void ringbuffer_reset(struct ringbuffer *rb);
unsigned int ringbuffer_put(struct ringbuffer *rb, const unsigned char *ptr, unsigned short length);
unsigned int ringbuffer_put_force(struct ringbuffer *rb, const unsigned char *ptr, unsigned short length);
unsigned int ringbuffer_putchar(struct ringbuffer *rb, const unsigned char ch);
unsigned int ringbuffer_putchar_force(struct ringbuffer *rb, const unsigned char ch);
unsigned int ringbuffer_get(struct ringbuffer *rb, unsigned char *ptr, unsigned short length);
unsigned int ringbuffer_getchar(struct ringbuffer *rb, unsigned char *ch);
unsigned int ringbuffer_data_len(struct ringbuffer *rb);

#ifdef USING_HEAP
struct ringbuffer* ringbuffer_create(uint16_t length);
void ringbuffer_destroy(struct ringbuffer *rb);
#endif

__inline unsigned short ringbuffer_get_size(struct ringbuffer *rb)
{
    //ASSERT(rb != RT_NULL);
    return rb->buffer_size;
}

/** return the size of empty space in rb */
#define ringbuffer_space_len(rb) ((rb)->buffer_size - ringbuffer_data_len(rb))


#ifdef __cplusplus
}
#endif

#endif
