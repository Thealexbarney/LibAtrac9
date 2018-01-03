#include "huffCodes.h"
#include "utility.h"

int ReadHuffmanValue(const HuffmanCodebook* huff, bit_reader_cxt* br, int is_signed)
{
	const int code = peek_int(br, huff->MaxBitSize);
	const unsigned char value = huff->Lookup[code];
	const int bits = huff->Bits[value];
	br->position += bits;
	return is_signed ? SignExtend32(value, huff->ValueBits) : value;
}

void DecodeHuffmanValues(int* spectrum, int index, int bandCount, const HuffmanCodebook* huff, const int* values)
{
	const int valueCount = bandCount >> huff->ValueCountPower;
	const int mask = (1 << huff->ValueBits) - 1;

	for (int i = 0; i < valueCount; i++)
	{
		int value = values[i];
		for (int j = 0; j < huff->ValueCount; j++)
		{
			spectrum[index++] = SignExtend32(value & mask, huff->ValueBits);
			value >>= huff->ValueBits;
		}
	}
}