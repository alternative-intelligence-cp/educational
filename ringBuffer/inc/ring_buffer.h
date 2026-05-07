#ifndef RING_BUFFER_H
    #define RING_BUFFER_H
    #include "boolean.h"
    #include "ring_buffer_errors.h"
    #define RING_BUFFER_NULL ((RingBuffer*)(0))
    
    typedef struct ring_buffer_t RingBuffer;
 
    typedef enum ring_buffer_compare_result_t{
        RB_CMP_LT=-1,RB_CMP_EQ=0,RB_CMP_GT=1
    }RingBufferCompareResult;
 
    typedef RingBufferCompareResult (*RingBuffer_compare)(void* a, void* b);
      
 
    RingBuffer* RingBuffer_create(unsigned long stride,unsigned long initialCapacity,Boolean dynamic);
    Boolean RingBuffer_destroy(RingBuffer* this);
    Boolean RingBuffer_isNull(RingBuffer* this);
    unsigned long RingBuffer_getStride(RingBuffer* this);
    unsigned long RingBuffer_getCount(RingBuffer* this);
    Boolean RingBuffer_isDynamic(RingBuffer* this);
    RingBufferErrors RingBuffer_getError(RingBuffer* this);
    
    Boolean RingBuffer_add(RingBuffer* this, unsigned long index, void*data);
    void* RingBuffer_remove(RingBuffer* this, unsigned long index);
    
    Boolean RingBuffer_pad(RingBuffer* this, void* data);
    Boolean RingBuffer_push(RingBuffer* this, void* data);
    
    void* RingBuffer_pull(RingBuffer* this);
    void* RingBuffer_pop(RingBuffer* this);
    
    void* RingBuffer_pry(RingBuffer* this);
    void* RingBuffer_peek(RingBuffer* this);
    
    void* RingBuffer_get(RingBuffer* this,unsigned long index);
    Boolean RingBuffer_set(RingBuffer* this, unsigned long index, void* data);
    
    unsigned long RingBuffer_find(RingBuffer* this, void* data);
    Boolean RingBuffer_contains(RingBuffer* this, void* data);
    
    //TODO vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    Boolean RingBuffer_sort(RingBuffer* this, RingBuffer_compare comparisonFunction);
    //TODO ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    
#endif
