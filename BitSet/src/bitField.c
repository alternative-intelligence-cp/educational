#include "bitField.h"

BitFieldOperationResult getBit(BitField bits, __UINT8_TYPE__ index) {
    BitFieldOperationResult result;
    if (index > 7) {
        result.err = 1;
        result.value = BIT_FIELD_NULL;
        return result;
    }
    result.err = 0;
    result.value = (bits & BIT_MASKS[index]) >> index;
    return result;
}

BitFieldOperationResult setBit(BitField* bits, __UINT8_TYPE__ index, Bit value) {
    BitFieldOperationResult result;
    if (index > 7) {
        result.err = 1;
        result.value = BIT_FIELD_NULL;
        return result;
    }
    if (value > 1) {
        result.err = 1;
        result.value = BIT_FIELD_NULL;
        return result;
    }
    result.err = 0;
    *bits = (*bits & ~BIT_MASKS[index]) | (value << index);
    result.value = *bits;
    return result;
}

BitFieldOperationResult flipBit(BitField* bits, __UINT8_TYPE__ index) {
    BitFieldOperationResult result;
    if (index > 7) {
        result.err = 1;
        result.value = BIT_FIELD_NULL;
        return result;
    }
    result.err = 0;
    *bits = *bits ^ BIT_MASKS[index];
    result.value = *bits;
    return result;
}

BitFieldOperationResult rotateLeft(BitField* bits, __UINT8_TYPE__ numBits) {
    BitFieldOperationResult result;
    if (numBits > 7) {
        result.err = 1;
        result.value = BIT_FIELD_NULL;
        return result;
    }
    result.err = 0;
    *bits = (*bits << numBits) | (*bits >> (8 - numBits));
    result.value = *bits;
    return result;
}

BitFieldOperationResult rotateRight(BitField* bits, __UINT8_TYPE__ numBits) {
    BitFieldOperationResult result;
    if (numBits > 7) {
        result.err = 1;
        result.value = BIT_FIELD_NULL;
        return result;
    }
    result.err = 0;
    *bits = (*bits >> numBits) | (*bits << (8 - numBits));
    result.value = *bits;
    return result;
}

BitFieldOperationResult shiftLeft(BitField* bits, __UINT8_TYPE__ numBits) {
    BitFieldOperationResult result;
    if (numBits > 7) {
        result.err = 1;
        result.value = BIT_FIELD_NULL;
        return result;
    }
    result.err = 0;
    *bits = (*bits << numBits);
    result.value = *bits;
    return result;
}

BitFieldOperationResult shiftRight(BitField* bits, __UINT8_TYPE__ numBits) {
    BitFieldOperationResult result;
    if (numBits > 7) {
        result.err = 1;
        result.value = BIT_FIELD_NULL;
        return result;
    }
    result.err = 0;
    *bits = (*bits >> numBits);
    result.value = *bits;
    return result;
}