#ifndef BIT_SET_H
    #define BIT_SET_H

    #include "bitField.h"
    #include "stdbool.h"
    #include "malloc.h"
    #include "memory.h"

    typedef struct bit_set_t {
        BitField** bits;
        __UINT64_TYPE__ length;
    } BitSet;

    __UINT64_TYPE__ calculateNumFields(__UINT64_TYPE__ numBits);

    BitSet* newBitSet(__UINT64_TYPE__ numBits);

    bool destroyBitSet(BitSet* bitSet);

    Bit getBitAtIndex(BitSet* bitSet, __UINT64_TYPE__ bit);

    bool setBitAtIndex(BitSet* bitSet,__UINT64_TYPE__ bit,__UINT8_TYPE__ value);

    bool isBitSetAtIndex(BitSet* bitSet,__UINT64_TYPE__ bit);

    bool flipBitAtIndex(BitSet* bitSet, __UINT64_TYPE__ bit);

    bool rotateBitSetLeft(BitSet* bitSet, __UINT64_TYPE__ numBits);

    bool rotateBitSetRight(BitSet* bitSet, __UINT64_TYPE__ numBits);

    bool shiftBitSetLeft(BitSet* bitSet, __UINT64_TYPE__ numBits);

    bool shiftBitSetRight(BitSet* bitSet, __UINT64_TYPE__ numBits);

    char* bitSetToString(BitSet* bitSet);


#endif