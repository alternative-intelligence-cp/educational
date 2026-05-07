#include "inc/ring_buffer_errors.h"
char* RingBufferErrors_toString(RingBufferErrors err){
    char* errStr;
    switch(err){
        case RB_ERR_NONE:
            errStr="NONE";
            break;
        case RB_ERR_EMPTY:
            errStr="EMPTY";
            break;
        case RB_ERR_FULL:
            errStr="FULL";
            break;
        case RB_ERR_OOM:
            errStr="OUT OF MEMORY";
            break;
        case RB_ERR_OOB:
            errStr="OUT OF BOUNDS";
            break;
        case RB_ERR_NOT_FOUND:
            errStr="NOT FOUND";
            break;
        case RB_ERR_NULL:
            errStr="NULL";
            break;
        case RB_ERR_UNKNOWN:
            errStr="UNKNOWN";
            break;
        default:
            errStr="INVALID ERROR TYPE";       
    }
    return errStr;
}
