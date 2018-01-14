#pragma once

typedef struct {
	const unsigned char * buffer;
	int position;
} bit_reader_cxt;

// Make MSVC compiler happy. Leave const in for value parameters

void init_bit_reader_cxt(bit_reader_cxt* br, const void * buffer);
int peek_int(bit_reader_cxt* br, const int bits);
int read_int(bit_reader_cxt* br, const int bits);
int read_signed_int(bit_reader_cxt* br, const int bits);
int read_offset_binary(bit_reader_cxt* br, const int bits);
void align_position(bit_reader_cxt* br, const unsigned int multiple);
