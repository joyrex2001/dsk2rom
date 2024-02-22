#include <stdio.h>
#include <stdlib.h>

struct Uncompressor {
    unsigned char* compressedData;
    int dataPosition;
    int varPosition;
    int bitForVarPosition;
};

void uncompress(struct Uncompressor* uncompressor, size_t in_data_length, unsigned char *out_data);
