#ifndef BIT_FIELD_H
#define BIT_FIELD_H

#define Bit __UINT8_TYPE__
#define BitField __UINT8_TYPE__
#define BIT_FIELD_NULL 0

static const __UINT8_TYPE__ BIT_MASKS[] = {
    1,    // 2^0 = 00000001
    2,    // 2^1 = 00000010
    4,    // 2^2 = 00000100
    8,    // 2^3 = 00001000
    16,   // 2^4 = 00010000
    32,   // 2^5 = 00100000
    64,   // 2^6 = 01000000
    128   // 2^7 = 10000000
};

typedef struct bit_field_operation_result_t {
    __UINT8_TYPE__ err;
    __UINT8_TYPE__ value;
} BitFieldOperationResult;

BitFieldOperationResult getBit(BitField bits, __UINT8_TYPE__ index);
BitFieldOperationResult setBit(BitField* bits, __UINT8_TYPE__ index, Bit value);
BitFieldOperationResult flipBit(BitField* bits, __UINT8_TYPE__ index);
BitFieldOperationResult rotateLeft(BitField* bits, __UINT8_TYPE__ numBits);
BitFieldOperationResult rotateRight(BitField* bits, __UINT8_TYPE__ numBits);
BitFieldOperationResult shiftLeft(BitField* bits, __UINT8_TYPE__ numBits);
BitFieldOperationResult shiftRight(BitField* bits, __UINT8_TYPE__ numBits);

#endif