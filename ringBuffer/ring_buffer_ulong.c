#include "inc/protected/ring_buffer_ulong_protected.h"
#include <malloc.h>
RingBufferULong* RingBufferULong_create(unsigned long initialCapacity,Boolean dynamic){
    RingBufferULong* newRBUL;
    void* temp = malloc(sizeof(RingBufferULong));
    if(temp == NULL){ return RING_BUFFER_ULONG_NULL; }
    newRBUL= (RingBufferULong*)temp;
    temp = RingBuffer_create(sizeof(unsigned long),initialCapacity,dynamic);
    if(temp == NULL){
        free(newRBUL);
        return RING_BUFFER_ULONG_NULL;
    }
    newRBUL->buffer= temp;
    return newRBUL;
}

Boolean RingBufferULong_destroy(RingBufferULong* this){
    RingBuffer_destroy(this->buffer);
    free(this);
    return TRUE;
}

Boolean RingBufferULong_isNull(RingBufferULong* this){
    return this == RING_BUFFER_ULONG_NULL;
}

unsigned long RingBufferULong_getStride(){
    return sizeof(unsigned long);
}

unsigned long RingBufferULong_getCount(RingBufferULong* this){
    return RingBuffer_getCount(this->buffer);
}

Boolean RingBufferULong_isDynamic(RingBufferULong* this){
    return RingBuffer_isDynamic(this->buffer);
}

RingBufferErrors RingBufferULong_getError(RingBufferULong* this){
    return RingBuffer_getError(this->buffer);
}

Boolean RingBufferULong_add(RingBufferULong* this, unsigned long index, unsigned long data){
    return RingBuffer_add(this->buffer,index,&data);
}
    
unsigned long RingBufferULong_remove(RingBufferULong* this, unsigned long index){
    return *(unsigned long*)RingBuffer_remove(this->buffer, index);
}


Boolean RingBufferULong_pad(RingBufferULong* this, unsigned long data){
    return RingBuffer_pad(this->buffer,&data);
}

Boolean RingBufferULong_push(RingBufferULong* this, unsigned long data){
    return RingBuffer_push(this->buffer,&data);
}

unsigned long RingBufferULong_pull(RingBufferULong* this){
    return *(unsigned long*)RingBuffer_pull(this->buffer);
}

unsigned long RingBufferULong_pop(RingBufferULong* this){
    return *(unsigned long*)RingBuffer_pop(this->buffer);
}

unsigned long RingBufferULong_pry(RingBufferULong* this){
    return *(unsigned long*)RingBuffer_pry(this->buffer);
}

unsigned long RingBufferULong_peek(RingBufferULong* this){
    return *(unsigned long*)RingBuffer_peek(this->buffer);
}

unsigned long RingBufferULong_get(RingBufferULong* this,unsigned long index){
    return *(unsigned long*)RingBuffer_get(this->buffer,index);
}

Boolean RingBufferULong_set(RingBufferULong* this, unsigned long index, unsigned long data){
    return RingBuffer_set(this->buffer,index,&data);   
}

unsigned long RingBufferULong_find(RingBufferULong* this, unsigned long data){
    return RingBuffer_find(this->buffer,&data);
}

Boolean RingBufferULong_contains(RingBufferULong* this, unsigned long data){
    return RingBuffer_contains(this->buffer,&data);
}

Boolean RingBufferULong_sort(RingBufferULong* this){
    return RingBuffer_sort(this->buffer,RingBufferULong_compare);
}

RingBufferCompareResult RingBufferULong_compare(void*a, void*b){
    unsigned long ula = *(unsigned long*)a;
    unsigned long ulb = *(unsigned long*)b;
    if(ula < ulb){
        return RB_CMP_LT;
    }
    if(ula > ulb){
        return RB_CMP_GT;
    }
    return RB_CMP_EQ;
}

