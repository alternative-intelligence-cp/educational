#ifndef RING_BUFFER_DOUBLE_H
    #define RING_BUFFER_DOUBLE_H
    #define RING_BUFFER_DOUBLE_NULL ((RingBufferDouble*)0)
    #include "boolean.h"
    #include "ring_buffer_errors.h"
    #include "ring_buffer.h"
    
    typedef struct ring_buffer_double_t RingBufferDouble;
    
    RingBufferDouble* RingBufferDouble_create(unsigned long initialCapacity, Boolean dynamic);
    Boolean RingBufferDouble_destroy(RingBufferDouble* this);
    Boolean RingBufferDouble_isNull(RingBufferDouble* this);
    unsigned long RingBufferDouble_getStride();
    unsigned long RingBufferDouble_getCount(RingBufferDouble* this);
    Boolean RingBufferDouble_isDynamic(RingBufferDouble* this);
    RingBufferErrors RingBufferDouble_getError(RingBufferDouble* this);
    
    Boolean RingBufferDouble_add(RingBufferDouble* this, unsigned long index, double data);
    double RingBufferDouble_remove(RingBufferDouble* this, unsigned long index);
    
    Boolean RingBufferDouble_pad(RingBufferDouble* this, double data);
    Boolean RingBufferDouble_push(RingBufferDouble* this,double data);
    
    double RingBufferDouble_pull(RingBufferDouble* this);
    double RingBufferDouble_pop(RingBufferDouble* this);
    
    double RingBufferDouble_pry(RingBufferDouble* this);
    double RingBufferDouble_peek(RingBufferDouble* this);
    
    double RingBufferDouble_get(RingBufferDouble* this,unsigned long index);
    Boolean RingBufferDouble_set(RingBufferDouble* this, unsigned long index, double data);
    
    unsigned long RingBufferDouble_find(RingBufferDouble* this, double data);
    Boolean RingBufferDouble_contains(RingBufferDouble* this, double data);
    
    Boolean RingBufferDouble_sort(RingBufferDouble* this);
    
    extern RingBufferCompareResult RingBufferDouble_compare(void* a, void* b);
    
#endif
