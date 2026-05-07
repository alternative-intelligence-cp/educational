#include "inc/protected/ring_buffer_protected.h"
#include <stdlib.h>
#include <string.h>

RingBuffer* RingBuffer_create(unsigned long stride, unsigned long initialCapacity, Boolean dynamic){
    void* temp = malloc(sizeof(RingBuffer));
    if(temp == NULL){ return RING_BUFFER_NULL; }
    RingBuffer* rb = (RingBuffer*)temp;
    rb->memory = (char*)calloc(initialCapacity, stride);
    if(rb->memory == NULL){
        free(rb);
        return RING_BUFFER_NULL;
    }
    rb->scratch = (char*)malloc(stride);
    if(rb->scratch == NULL){
        free(rb->memory);
        free(rb);
        return RING_BUFFER_NULL;
    }
    rb->stride = stride;
    rb->capacity = initialCapacity;
    rb->count = 0;
    rb->head = 0;
    rb->dynamic = dynamic;
    rb->error = RB_ERR_NONE;
    return rb;
}

Boolean RingBuffer_destroy(RingBuffer* this){
    if(this == RING_BUFFER_NULL){ return FALSE; }
    free(this->scratch);
    free(this->memory);
    free(this);
    return TRUE;
}

Boolean RingBuffer_isNull(RingBuffer* this){
    return this == RING_BUFFER_NULL ? TRUE : FALSE;
}

unsigned long RingBuffer_getStride(RingBuffer* this){
    return this->stride;
}

unsigned long RingBuffer_getCount(RingBuffer* this){
    return this->count;
}

RingBufferErrors RingBuffer_getError(RingBuffer* this){
    return this->error;
}

Boolean RingBuffer_isDynamic(RingBuffer* this){
    return this->dynamic == TRUE ? TRUE : FALSE;
}

Boolean RingBuffer_grow(RingBuffer* this){
    if(this->dynamic == FALSE){ 
        this->error=RB_ERR_FULL; 
        return FALSE; 
    }
    char* newmem = (char*)malloc(this->stride*(this->capacity *2));
    if(newmem == NULL){
        this->error=RB_ERR_OOM; 
        return FALSE; 
    }
    unsigned long target = this->head * this->stride;
    unsigned long max = this->capacity * this->stride;
    for(unsigned long i = 0; i < this->count * this->stride; i++){
       if(target >= max){
            target = 0;
       }
       newmem[i] = this->memory[target];
       target++;
    }
    free(this->memory);
    this->memory = newmem;
    this->capacity *= 2;
    this->error=RB_ERR_NONE;
    this->head=0;
    return TRUE;
}

Boolean RingBuffer_add(RingBuffer* this, unsigned long index, void* data){
    if(index == 0){ return RingBuffer_pad(this, data); }
    if(index == this->count){ return RingBuffer_push(this, data); }
    if(index > this->count){
        this->error = RB_ERR_OOB;
        return FALSE;
    }
    if(this->count == this->capacity){
        if(RingBuffer_grow(this) != TRUE){
            //this->error was set by grow, no need to redo
            return FALSE;
        }
    }
    // Shift whichever half is shorter to minimize element copies.
    if(index <= this->count / 2){
        // Front half is shorter: shift front elements one slot toward the tail
        // so we can move head back and write the new element into the freed slot.
        unsigned long new_head = (this->head == 0) ? this->capacity - 1 : this->head - 1;
        for(unsigned long i = 0; i < index; i++){
            unsigned long src = (this->head + i) % this->capacity;
            unsigned long dst = (new_head + i) % this->capacity;
            memcpy(&this->memory[dst * this->stride], &this->memory[src * this->stride], this->stride);
        }
        this->head = new_head;
    } else {
        // Back half is shorter: shift back elements one slot toward the tail
        // to free up the slot at logical position `index`.
        for(unsigned long i = this->count; i > index; i--){
            unsigned long src = (this->head + i - 1) % this->capacity;
            unsigned long dst = (this->head + i) % this->capacity;
            memcpy(&this->memory[dst * this->stride], &this->memory[src * this->stride], this->stride);
        }
    }
    unsigned long target = (this->head + index) % this->capacity;
    memcpy(&this->memory[target * this->stride], data, this->stride);
    this->count++;
    this->error = RB_ERR_NONE;
    return TRUE;
}

void* RingBuffer_remove(RingBuffer* this, unsigned long index){
    if(this->count == 0){
        this->error = RB_ERR_EMPTY;
        return NULL;
    }
    if(index >= this->count){
        this->error = RB_ERR_OOB;
        return NULL;
    }
    // Copy removed element to scratch before any shifting overwrites it,
    // so the returned pointer stays valid until the next operation.
    unsigned long phys = (this->head + index) % this->capacity;
    memcpy(this->scratch, &this->memory[phys * this->stride], this->stride);
    if(index == 0){
        this->head = (this->head + 1) % this->capacity;
        this->count--;
        if(this->count == 0){ this->head = 0; }
    } else if(index == this->count - 1){
        this->count--;
        if(this->count == 0){ this->head = 0; }
    } else if(index <= this->count / 2){
        // Front half is shorter: shift front elements one slot toward the tail,
        // then advance head to discard the duplicated front slot.
        for(unsigned long i = index; i > 0; i--){
            unsigned long src = (this->head + i - 1) % this->capacity;
            unsigned long dst = (this->head + i) % this->capacity;
            memcpy(&this->memory[dst * this->stride], &this->memory[src * this->stride], this->stride);
        }
        this->head = (this->head + 1) % this->capacity;
        this->count--;
        if(this->count == 0){ this->head = 0; }
    } else {
        // Back half is shorter: shift back elements one slot toward the head.
        for(unsigned long i = index; i < this->count - 1; i++){
            unsigned long src = (this->head + i + 1) % this->capacity;
            unsigned long dst = (this->head + i) % this->capacity;
            memcpy(&this->memory[dst * this->stride], &this->memory[src * this->stride], this->stride);
        }
        this->count--;
        if(this->count == 0){ this->head = 0; }
    }
    this->error = RB_ERR_NONE;
    return this->scratch;
}

Boolean RingBuffer_pad(RingBuffer* this, void* data){
    if(this->count == this->capacity){
        if(RingBuffer_grow(this) != TRUE){
            //this->error was set by grow, no need to redo 
            return FALSE;
        }
    }
    char* src = (char*)data;
    unsigned long target= this->head == 0 ? this->capacity - 1 : this->head - 1; 
    for(unsigned long i = 0; i < this->stride; i++){
        this->memory[(target * this->stride) +i] = src[i];
    }
    this->head = target;
    this->count++;
    this->error = RB_ERR_NONE;
    return TRUE;
}

Boolean RingBuffer_push(RingBuffer* this, void* data){
    if(this->count == this->capacity){
        if(RingBuffer_grow(this) != TRUE){
            //this->error was set by grow, no need to redo 
            return FALSE;
        }
    }
    char* src = (char*)data;
    unsigned long target= this->head + this->count;
    if(target >= this->capacity){target = target - this->capacity;}
    for(unsigned long i = 0; i < this->stride; i++){
        this->memory[(target * this->stride) + i] = src[i];
    }
    this->count++;
    this->error = RB_ERR_NONE;
    return TRUE;
}

void* RingBuffer_pull(RingBuffer* this){
    if(this->count > 0){
        unsigned long index = this->head;
        if(index >= this->capacity){index = index - this->capacity;}
        this->count--;
        if(this->count == 0){
            this->head = 0;
        } else {
            if(this->head +1 == this->capacity){
                this->head=0;
            } else {
                this->head++;
            }
        }
        this->error = RB_ERR_NONE;
        return &this->memory[index*this->stride];
    } else {
        this->error=RB_ERR_EMPTY;
        return NULL;
    }
}

void* RingBuffer_pop(RingBuffer* this){
    if(this->count > 0){
        unsigned long index = this->head + (this->count -1);
        if(index >= this->capacity){index = index - this->capacity;}
        this->count--;
        if(this->count == 0){
            this->head = 0;
        }
        this->error = RB_ERR_NONE;
        return &this->memory[index * this->stride];
    } else {
        this->error=RB_ERR_EMPTY;
        return NULL;
    }
}

void* RingBuffer_pry(RingBuffer* this){
    if(this->count > 0){
        this->error = RB_ERR_NONE;
        return &this->memory[this->head * this->stride];
    } else {
        this->error=RB_ERR_EMPTY;
        return NULL;
    }
}

void* RingBuffer_peek(RingBuffer* this){
    if(this->count > 0){
        this->error = RB_ERR_NONE;
        unsigned long tail = (this->head + (this->count - 1)) % this->capacity;
        return &this->memory[tail * this->stride];
    } else {
        this->error = RB_ERR_EMPTY;
        return NULL;
    }
}

void* RingBuffer_get(RingBuffer* this, unsigned long index){
    if(this->count == 0){
        this->error = RB_ERR_EMPTY;
        return NULL;
    }
    if(index >= this->count){
        this->error = RB_ERR_OOB;
        return NULL;
    }
    unsigned long loc = (this->head + index) % this->capacity;
    this->error = RB_ERR_NONE;
    return &this->memory[loc * this->stride];
}

Boolean RingBuffer_set(RingBuffer* this, unsigned long index, void* data){
    if(this->count == 0){
        this->error = RB_ERR_EMPTY;
        return FALSE;
    }
    if(index >= this->count){
        this->error = RB_ERR_OOB;
        return FALSE;
    }
    unsigned long loc = (this->head + index) % this->capacity;
    memcpy(&this->memory[loc * this->stride], data, this->stride);
    this->error = RB_ERR_NONE;
    return TRUE;
}

unsigned long RingBuffer_find(RingBuffer* this, void* data){
    if(this->count == 0){
        this->error = RB_ERR_EMPTY;
        return 0;
    }
    for(unsigned long i = 0; i < this->count; i++){
        unsigned long phys = (this->head + i) % this->capacity;
        if(memcmp(&this->memory[phys * this->stride], data, this->stride) == 0){
            this->error = RB_ERR_NONE;
            return i;
        }
    }
    this->error = RB_ERR_NOT_FOUND;
    return 0;
}

Boolean RingBuffer_contains(RingBuffer* this, void* data){
    RingBuffer_find(this,data);
    return this->error == RB_ERR_NONE ? TRUE : FALSE;
}

Boolean RingBuffer_sort(RingBuffer* this, RingBuffer_compare comparisonFunction){
    if(this == RING_BUFFER_NULL){ return FALSE; }
    if(this->count <= 1){ this->error = RB_ERR_NONE; return TRUE; }

    // Insertion sort: simple, in-place, and easy to follow.
    // For each element starting at position 1, shift it left past any
    // elements that are greater than it until it finds its sorted position.
    for(unsigned long i = 1; i < this->count; i++){
        // Copy element i into scratch as the "key" to place.
        unsigned long phys_i = (this->head + i) % this->capacity;
        memcpy(this->scratch, &this->memory[phys_i * this->stride], this->stride);
        long j = (long)i - 1;
        while(j >= 0){
            unsigned long phys_j = (this->head + (unsigned long)j) % this->capacity;
            if(comparisonFunction(&this->memory[phys_j * this->stride], this->scratch) == RB_CMP_GT){
                // Element at j is greater than the key; shift it one slot right.
                unsigned long phys_j1 = (this->head + (unsigned long)(j + 1)) % this->capacity;
                memcpy(&this->memory[phys_j1 * this->stride], &this->memory[phys_j * this->stride], this->stride);
                j--;
            } else {
                break;
            }
        }
        // Place the key in its correct sorted position.
        unsigned long dest = (this->head + (unsigned long)(j + 1)) % this->capacity;
        memcpy(&this->memory[dest * this->stride], this->scratch, this->stride);
    }
    this->error = RB_ERR_NONE;
    return TRUE;
}

