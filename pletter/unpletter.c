/* Modified by Sylver Bruneau from code by Sylvain Glaize : */
/* https://gitea.zaclys.com/Mokona/Unpletter/               */

#include <stdio.h>
#include <stdlib.h>
#include "unpletter.h"

typedef unsigned char data_type;

static data_type getByte(struct Uncompressor* uncompressor)
{
    return uncompressor->compressedData[uncompressor->dataPosition++];
}

static data_type getBit(struct Uncompressor* uncompressor)
{
    if (uncompressor->bitForVarPosition == 7)
    {
        uncompressor->varPosition = uncompressor->dataPosition;
        uncompressor->dataPosition = uncompressor->varPosition + 1;
    }

    data_type bit = (uncompressor->compressedData[uncompressor->varPosition] >> uncompressor->bitForVarPosition) & 1;
    uncompressor->bitForVarPosition--;

    if (uncompressor->bitForVarPosition == -1)
    {
        uncompressor->bitForVarPosition = 7;
    }
    return bit;
}

static int getInterlacedEliasGamma(struct Uncompressor* uncompressor)
{
    int value = 1;
    while (getBit(uncompressor))
    {
        value = (value << 1) | getBit(uncompressor);
    }
    return value;
}

unsigned char * realloc_output(unsigned char * output, size_t output_position, size_t * output_max_size, size_t add_data_size)
{
    if (output_position + add_data_size > *output_max_size)
    {
        *output_max_size *= 2;
        output = (unsigned char*) realloc(output, *output_max_size);
    }
    return output;
}

void uncompress(struct Uncompressor* uncompressor, size_t in_data_length, unsigned char *out_data)
{
    size_t output_position = 0;

    data_type first_byte = getByte(uncompressor);
    out_data[output_position++] = first_byte;

    while (uncompressor->dataPosition < in_data_length)
    {
        if (getBit(uncompressor))
        {
            int length = getInterlacedEliasGamma(uncompressor) + 1;

            if (length == 262143 || (length == 131072))
            {
                break;
            }

            int offset = getByte(uncompressor);
            if (offset & 0x80)
            {
                offset = offset & 0x7F;
                offset = offset | (getBit(uncompressor) << 8);
                offset = offset | (getBit(uncompressor) << 7);
                offset += 128;
            }
            offset += 1;

            for (int i = 0; i < length; i++)
            {
                out_data[output_position] = out_data[output_position - offset];
                output_position += 1;
            }
        }
        else
        {
            out_data[output_position++] = getByte(uncompressor);
        }
        if (output_position >= 512) break;
    }
}
