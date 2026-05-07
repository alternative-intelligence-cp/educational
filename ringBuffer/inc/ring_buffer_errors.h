#ifndef RING_BUFFER_ERRORS_H
    #define RING_BUFFER_ERRORS_H
    
    typedef enum ring_buffer_errors_t {
        RB_ERR_NONE=0,
        RB_ERR_EMPTY=1,
        RB_ERR_FULL=2,
        RB_ERR_OOM=3,
        RB_ERR_OOB=4,
        RB_ERR_NOT_FOUND=5,
        RB_ERR_NULL=254,
        RB_ERR_UNKNOWN=255
    }RingBufferErrors;
    
    char* RingBufferErrors_toString(RingBufferErrors err);
    
#endif
