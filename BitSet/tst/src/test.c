#include <stdio.h>
#include "bitField.h"
#include "bitSet.h"

int main(int argc, char** argv) {
    BitSet* bitSet = newBitSet(16);

    printf("Set index 0 and 15\n");
    setBitAtIndex(bitSet, 0, 1);
    setBitAtIndex(bitSet, 15, 1);
    char* bitSetStr = bitSetToString(bitSet);
    printf("bitSet: %s\n", bitSetStr);

    printf("Flip index 1 and 14\n");
    flipBitAtIndex(bitSet, 1);
    flipBitAtIndex(bitSet, 14);
    bitSetStr = bitSetToString(bitSet);
    printf("bitSet: %s\n", bitSetStr);

    printf("Rotate right\n");
    rotateBitSetRight(bitSet, 1);
    bitSetStr = bitSetToString(bitSet);
    printf("bitSet: %s\n", bitSetStr);

    printf("Rotate left\n");
    rotateBitSetLeft(bitSet, 1);
    bitSetStr = bitSetToString(bitSet);
    printf("bitSet: %s\n", bitSetStr);

    printf("Rotate left\n");
    rotateBitSetLeft(bitSet, 1);
    bitSetStr = bitSetToString(bitSet);
    printf("bitSet: %s\n", bitSetStr);

    printf("Rotate right\n");
    rotateBitSetRight(bitSet, 1);
    bitSetStr = bitSetToString(bitSet);
    printf("bitSet: %s\n", bitSetStr);

    printf("Shift left\n");
    shiftBitSetLeft(bitSet, 1);
    bitSetStr = bitSetToString(bitSet);
    printf("bitSet: %s\n", bitSetStr);

    printf("Shift left\n");
    shiftBitSetLeft(bitSet, 1);
    bitSetStr = bitSetToString(bitSet);
    printf("bitSet: %s\n", bitSetStr);

    printf("Shift right\n");
    shiftBitSetRight(bitSet, 2);
    bitSetStr = bitSetToString(bitSet);
    printf("bitSet: %s\n", bitSetStr);

    printf("Shift right\n");
    shiftBitSetRight(bitSet, 2);
    bitSetStr = bitSetToString(bitSet);
    printf("bitSet: %s\n", bitSetStr);

    destroyBitSet(bitSet);
}
