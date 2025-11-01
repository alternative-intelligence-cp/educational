#include "bitSet.h"

__UINT64_TYPE__ calculateNumFields(__UINT64_TYPE__ numBits) {
    return numBits / 8 + 1;
}

BitSet* newBitSet(__UINT64_TYPE__ numBits) {
    BitSet* bitSet = (BitSet*)malloc(sizeof(BitSet));
    if (bitSet == NULL) {
        return NULL;
    }
    bitSet->length = numBits;
    bitSet->bits = (BitField**)malloc(sizeof(BitField*) * calculateNumFields(numBits));
    if (bitSet->bits == NULL) {
        free(bitSet);
        return NULL;
    }
    for (__UINT64_TYPE__ i = 0; i < calculateNumFields(numBits); i++) {
        bitSet->bits[i] = (BitField*)malloc(sizeof(BitField));
        if (bitSet->bits[i] == NULL) {
            for (__UINT64_TYPE__ j = 0; j < i; j++) {
                free(bitSet->bits[j]);
            }
            free(bitSet->bits);
            free(bitSet);
            return NULL;
        }
        *(bitSet->bits[i]) = BIT_FIELD_NULL;
    }
    return bitSet;
}

bool destroyBitSet(BitSet* bitSet) {
    if (bitSet == NULL) {
        return false;
    }
    for (__UINT64_TYPE__ i = 0; i < calculateNumFields(bitSet->length); i++) {
        free(bitSet->bits[i]);
    }
    free(bitSet->bits);
    free(bitSet);
    return true;
}

Bit getBitAtIndex(BitSet* bitSet, __UINT64_TYPE__ bit) {
    if (bitSet == NULL) {
        return BIT_FIELD_NULL;
    }
    if (bit >= bitSet->length) {
        return BIT_FIELD_NULL;
    }
    BitFieldOperationResult result = getBit(*(bitSet->bits[bit / 8]), bit % 8);
    if (result.err) {
        return BIT_FIELD_NULL;
    }
    return result.value;
}

bool setBitAtIndex(BitSet* bitSet, __UINT64_TYPE__ bit, __UINT8_TYPE__ value) {
    if (bitSet == NULL) {
        return false;
    }
    if (bit >= bitSet->length) {
        return false;
    }
    BitFieldOperationResult result = setBit(bitSet->bits[bit / 8], bit % 8, value);
    if (result.err) {
        return false;
    }
    return true;
}

bool isBitSetAtIndex(BitSet* bitSet, __UINT64_TYPE__ bit) {
    if (bitSet == NULL) {
        return false;
    }
    if (bit >= bitSet->length) {
        return false;
    }
    BitFieldOperationResult result = getBit(*(bitSet->bits[bit / 8]), bit % 8);
    if (result.err) {
        return false;
    }
    return result.value;
}

bool flipBitAtIndex(BitSet* bitSet, __UINT64_TYPE__ bit) {
    if (bitSet == NULL) {
        return false;
    }
    if (bit >= bitSet->length) {
        return false;
    }
    BitFieldOperationResult result = flipBit(bitSet->bits[bit / 8], bit % 8);
    if (result.err) {
        return false;
    }
    return true;
}

bool rotateBitSetLeft(BitSet* bitSet, __UINT64_TYPE__ numBits) {
    if (bitSet == NULL || numBits == 0) {
        return false;
    }
    __UINT64_TYPE__ totalBits = bitSet->length;
    numBits = numBits % totalBits; // Normalize numBits to be within the length of the bit set
    if (numBits == 0) {
        return true;
    }

    char* temp = bitSetToString(bitSet);
    if (temp == NULL) {
        return false;
    }

    char* rotated = (char*)malloc(totalBits + 1);
    if (rotated == NULL) {
        free(temp);
        return false;
    }

    for (__UINT64_TYPE__ i = 0; i < totalBits; i++) {
        rotated[i] = temp[(i + numBits) % totalBits];
    }
    rotated[totalBits] = '\0';

    for (__UINT64_TYPE__ i = 0; i < totalBits; i++) {
        setBitAtIndex(bitSet, i, rotated[i] - '0');
    }

    free(temp);
    free(rotated);
    return true;
}

bool rotateBitSetRight(BitSet* bitSet, __UINT64_TYPE__ numBits) {
    if (bitSet == NULL || numBits == 0) {
        return false;
    }
    __UINT64_TYPE__ totalBits = bitSet->length;
    numBits = numBits % totalBits; // Normalize numBits to be within the length of the bit set
    if (numBits == 0) {
        return true;
    }

    char* temp = bitSetToString(bitSet);
    if (temp == NULL) {
        return false;
    }

    char* rotated = (char*)malloc(totalBits + 1);
    if (rotated == NULL) {
        free(temp);
        return false;
    }

    for (__UINT64_TYPE__ i = 0; i < totalBits; i++) {
        rotated[i] = temp[(i + totalBits - numBits) % totalBits];
    }
    rotated[totalBits] = '\0';

    for (__UINT64_TYPE__ i = 0; i < totalBits; i++) {
        setBitAtIndex(bitSet, i, rotated[i] - '0');
    }

    free(temp);
    free(rotated);
    return true;
}

bool shiftBitSetLeft(BitSet* bitSet, __UINT64_TYPE__ numBits) {
    if (bitSet == NULL || numBits == 0) {
        return false;
    }
    __UINT64_TYPE__ totalBits = bitSet->length;
    numBits = numBits % totalBits; // Normalize numBits to be within the length of the bit set
    if (numBits == 0) {
        return true;
    }

    char* temp = bitSetToString(bitSet);
    if (temp == NULL) {
        return false;
    }

    char* shifted = (char*)malloc(totalBits + 1);
    if (shifted == NULL) {
        free(temp);
        return false;
    }

    for (__UINT64_TYPE__ i = 0; i < totalBits; i++) {
        shifted[i] = i + numBits < totalBits ? temp[i + numBits] : '0';
    }
    shifted[totalBits] = '\0';

    for (__UINT64_TYPE__ i = 0; i < totalBits; i++) {
        setBitAtIndex(bitSet, i, shifted[i] - '0');
    }

    free(temp);
    free(shifted);
    return true;
}

bool shiftBitSetRight(BitSet* bitSet, __UINT64_TYPE__ numBits) {
    if (bitSet == NULL || numBits == 0) {
        return false;
    }
    __UINT64_TYPE__ totalBits = bitSet->length;
    numBits = numBits % totalBits; // Normalize numBits to be within the length of the bit set
    if (numBits == 0) {
        return true;
    }

    char* temp = bitSetToString(bitSet);
    if (temp == NULL) {
        return false;
    }

    char* shifted = (char*)malloc(totalBits + 1);
    if (shifted == NULL) {
        free(temp);
        return false;
    }

    for (__UINT64_TYPE__ i = 0; i < totalBits; i++) {
        shifted[i] = i >= numBits ? temp[i - numBits] : '0';
    }
    shifted[totalBits] = '\0';

    for (__UINT64_TYPE__ i = 0; i < totalBits; i++) {
        setBitAtIndex(bitSet, i, shifted[i] - '0');
    }

    free(temp);
    free(shifted);
    return true;
}

char* bitSetToString(BitSet* bitSet) {
    if (bitSet == NULL) {
        return NULL;
    }
    char* str = (char*)malloc(sizeof(char) * bitSet->length + 1);
    if (str == NULL) {
        return NULL;
    }
    for (__UINT64_TYPE__ i = 0; i < bitSet->length; i++) {
        BitFieldOperationResult result = getBit(*(bitSet->bits[i / 8]), i % 8);
        if (result.err) {
            free(str);
            return NULL;
        }
        str[i] = result.value + '0';
    }
    str[bitSet->length] = '\0';
    return str;
}