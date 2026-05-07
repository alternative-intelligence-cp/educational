#include "inc/protected/ring_buffer_double_protected.h"
#include <malloc.h>
RingBufferDouble* RingBufferDouble_create(unsigned long initialCapacity,Boolean dynamic){
    RingBufferDouble* newRBD;
    void* temp = malloc(sizeof(RingBufferDouble));
    if(temp == NULL){ return RING_BUFFER_DOUBLE_NULL; }
    newRBD= (RingBufferDouble*)temp;
    temp = RingBuffer_create(sizeof(double),initialCapacity,dynamic);
    if(temp == NULL){
        free(newRBD);
        return RING_BUFFER_DOUBLE_NULL;
    }
    newRBD->buffer= temp;
    return newRBD;
}

Boolean RingBufferDouble_destroy(RingBufferDouble* this){
    RingBuffer_destroy(this->buffer);
    free(this);
    return TRUE;
}

Boolean RingBufferDouble_isNull(RingBufferDouble* this){
    return this == RING_BUFFER_DOUBLE_NULL;
}

unsigned long RingBufferDouble_getStride(){
    return sizeof(double);
}

unsigned long RingBufferDouble_getCount(RingBufferDouble* this){
    return RingBuffer_getCount(this->buffer);
}

Boolean RingBufferDouble_isDynamic(RingBufferDouble* this){
    return RingBuffer_isDynamic(this->buffer);
}

RingBufferErrors RingBufferDouble_getError(RingBufferDouble* this){
    return RingBuffer_getError(this->buffer);
}

Boolean RingBufferDouble_add(RingBufferDouble* this, unsigned long index, double data){
    return RingBuffer_add(this->buffer,index,&data);
}

double RingBufferDouble_remove(RingBufferDouble* this, unsigned long index){
    return *(double*)RingBuffer_remove(this->buffer,index);
}

Boolean RingBufferDouble_pad(RingBufferDouble* this, double data){
    return RingBuffer_pad(this->buffer,&data);
}

Boolean RingBufferDouble_push(RingBufferDouble* this, double data){
    return RingBuffer_push(this->buffer,&data);
}

double RingBufferDouble_pull(RingBufferDouble* this){
    return *(double*)RingBuffer_pull(this->buffer);
}

double RingBufferDouble_pop(RingBufferDouble* this){
    return *(double*)RingBuffer_pop(this->buffer);
}

double RingBufferDouble_pry(RingBufferDouble* this){
    return *(double*)RingBuffer_pry(this->buffer);
}

double RingBufferDouble_peek(RingBufferDouble* this){
    return *(double*)RingBuffer_peek(this->buffer);
}

double RingBufferDouble_get(RingBufferDouble* this,unsigned long index){
    return *(double*)RingBuffer_get(this->buffer,index);
}

Boolean RingBufferDouble_set(RingBufferDouble* this, unsigned long index, double data){
    return RingBuffer_set(this->buffer,index,&data);
}

unsigned long RingBufferDouble_find(RingBufferDouble* this, double data){
    return RingBuffer_find(this->buffer,&data);
}

Boolean RingBufferDouble_contains(RingBufferDouble* this, double data){
    return RingBuffer_contains(this->buffer,&data);
}

Boolean RingBufferDouble_sort(RingBufferDouble* this){
    return RingBuffer_sort(this->buffer,RingBufferDouble_compare);
}

RingBufferCompareResult RingBufferDouble_compare(void* a, void* b){
    double da = *(double*)a;
    double db = *(double*)b;
    if(da < db){
        return RB_CMP_LT;
    }
    if(da > db){
        return RB_CMP_GT;
    }
    return RB_CMP_EQ;
}

