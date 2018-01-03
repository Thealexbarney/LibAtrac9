#include "utility.h"
#include <limits.h>

int max(int a, int b) { return a > b ? a : b; }
int min(int a, int b) { return a > b ? b : a; }

int SignExtend32(int value, int bits)
{
	const int shift = 8 * sizeof(int) - bits;
	return (value << shift) >> shift;
}

short Clamp16(int value)
{
	if (value > SHRT_MAX)
		return SHRT_MAX;
	if (value < SHRT_MIN)
		return SHRT_MIN;
	return (short)value;
}