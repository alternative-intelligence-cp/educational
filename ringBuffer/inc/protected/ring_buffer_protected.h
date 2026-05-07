#ifndef RING_BUFFER_PROTECTED_H
    #define RING_BUFFER_PROTECTED_H
    #include "../ring_buffer.h"
     
    typedef struct ring_buffer_t {
        unsigned long stride;
        unsigned long capacity;
        unsigned long count;
        unsigned long head;
        Boolean dynamic;
        RingBufferErrors error;
        char* memory;
        char* scratch;
    }RingBuffer;
        
#endif
