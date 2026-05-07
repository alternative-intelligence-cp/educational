#include "inc/ring_buffer.h"
#include "inc/ring_buffer_ulong.h"
#include "inc/ring_buffer_double.h"
#include <stdio.h>

void printRB(RingBufferULong* this){
    printf("RingBuffer: {\n\tStride: %lu,\n\tCount: %lu,\n\tDynamic: %s,\n\tError: %s\n}\n",RingBufferULong_getStride(this),RingBufferULong_getCount(this),RingBufferULong_isDynamic(this) == TRUE ? "TRUE" : "FALSE", RingBufferErrors_toString(RingBufferULong_getError(this))); 
}
int main(void){
    //RingBufferErrors Test ////////////////////////////////////////////////////
    printf("RB_ERRS: { \n\tNONE: %d,\n\tEMPTY: %d,\n\tFULL: %d,\n\tOOM: %d,\n\tOOB: %d,\n\tNOT_FOUND: %d,\n\tNULL: %d,\n\tUNKNOWN: %d\n}\n",RB_ERR_NONE,RB_ERR_EMPTY,RB_ERR_FULL,RB_ERR_OOM,RB_ERR_OOB,RB_ERR_NOT_FOUND,RB_ERR_NULL,RB_ERR_UNKNOWN);
    //RingBufferULong Test /////////////////////////////////////////////////////
    RingBufferULong* rbul = RingBufferULong_create(1,FALSE);
    RingBufferULong_push(rbul, 1234567890);
    printf("RingBufferULong_pop(rbul)--> %lu\n", RingBufferULong_pop(rbul));
    RingBufferULong_destroy(rbul);
    //RingBufferDouble Test ////////////////////////////////////////////////////
    RingBufferDouble* rbd=RingBufferDouble_create(1,TRUE);
    RingBufferDouble_push(rbd,123.456789);
    RingBufferDouble_push(rbd,111.456789);
    RingBufferDouble_push(rbd,110.000000);
    RingBufferDouble_pad(rbd,20);
    RingBufferDouble_pad(rbd,10);
    RingBufferDouble_remove(rbd,0);
    RingBufferDouble_add(rbd,3,99.111);
    printf("RingBufferDouble_pull(rbd)--> %f\n", RingBufferDouble_pull(rbd));
    printf("RingBufferDouble_pop(rbd)--> %f\n", RingBufferDouble_pop(rbd));
    printf("RingBufferDouble_getCount(rbd)--> %lu\n", RingBufferDouble_getCount(rbd));
    printf("RingBufferDouble_pop(rbd)--> %f\n", RingBufferDouble_pop(rbd));
    printf("RingBufferDouble_getError(rbd)--> %u\n",RingBufferDouble_getError(rbd));
    printf("RingBufferDouble_getCount(rbd)--> %lu\n", RingBufferDouble_getCount(rbd));
    printf("RingBufferDouble_pop(rbd)--> %f\n", RingBufferDouble_pop(rbd));
    printf("RingBufferDouble_getCount(rbd)--> %lu\n", RingBufferDouble_getCount(rbd));
    printf("RingBufferDouble_pull(rbd)--> %f\n", RingBufferDouble_pull(rbd));
    printf("RingBufferDouble_getCount(rbd)--> %lu\n", RingBufferDouble_getCount(rbd));
    RingBufferDouble_destroy(rbd);
    //compare test /////////////////////////////////////////////////////////////
    double a = 125;
    double b = 124;
    RingBufferCompareResult res = RingBufferDouble_compare(&a,&b);
    char* cmpStr;
    if(res == RB_CMP_LT){ cmpStr="LESS THAN"; }
    else if(res == RB_CMP_GT){ cmpStr="GREATER THAN";}    
    else{ cmpStr="EQUAL TO"; }
    printf("%f is %s %f\n",a,cmpStr,b);
    
}
