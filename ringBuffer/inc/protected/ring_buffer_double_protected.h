#ifndef RING_BUFFER_DOUBLE_PROTECTED_H
    #define RING_BUFFER_DOUBLE_PROTECTED_H
    #include "ring_buffer_protected.h"
    #include "../ring_buffer_double.h"
    typedef struct ring_buffer_double_t{
        RingBuffer* buffer;
    }RingBufferDouble;
#endif
