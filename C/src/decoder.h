#pragma once

#include "error_codes.h"
#include "structures.h"

at9_status Decode(atrac9_handle* handle, const unsigned char* audio, unsigned char* pcm, int* bytesUsed);
int GetCodecInfo(atrac9_handle* handle, CodecInfo* pCodecInfo);
