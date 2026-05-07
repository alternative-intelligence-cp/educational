#ifndef RING_BUFFER_ULONG_H
    #define RING_BUFFER_ULONG_H
    #define RING_BUFFER_ULONG_NULL ((RingBufferULong*)0)
    #include "boolean.h"
    #include "ring_buffer_errors.h"
    #include "ring_buffer.h"
    
    typedef struct ring_buffer_ulong_t RingBufferULong;
    
    RingBufferULong* RingBufferULong_create(unsigned long initialCapacity, Boolean dynamic);
    Boolean RingBufferULong_destroy(RingBufferULong* this);
    Boolean RingBufferULong_isNull(RingBufferULong* this);
    unsigned long RingBufferULong_getStride();
    unsigned long RingBufferULong_getCount(RingBufferULong* this);
    Boolean RingBufferULong_isDynamic(RingBufferULong* this);
    RingBufferErrors RingBufferULong_getError(RingBufferULong* this);
    
    Boolean RingBufferULong_add(RingBufferULong* this, unsigned long index, unsigned long data);
    unsigned long RingBufferULong_remove(RingBufferULong* this, unsigned long index);
    
    Boolean RingBufferULong_pad(RingBufferULong* this, unsigned long data);
    Boolean RingBufferULong_push(RingBufferULong* this, unsigned long data);
    
    unsigned long RingBufferULong_pull(RingBufferULong* this);
    unsigned long RingBufferULong_pop(RingBufferULong* this);
    
    unsigned long RingBufferULong_pry(RingBufferULong* this);
    unsigned long RingBufferULong_peek(RingBufferULong* this);
    
    unsigned long RingBufferULong_get(RingBufferULong* this,unsigned long index);
    Boolean RingBufferULong_set(RingBufferULong* this, unsigned long index, unsigned long data);
    
    unsigned long RingBufferULong_find(RingBufferULong* this, unsigned long data);
    Boolean RingBufferULong_contains(RingBufferULong* this, unsigned long data);
    
    Boolean RingBufferULong_sort(RingBufferULong* this);
    
    extern RingBufferCompareResult RingBufferULong_compare(void* a, void* b);
    
#endif
