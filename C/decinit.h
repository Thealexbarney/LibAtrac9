#pragma once

#include "error_codes.h"
#include "structures.h"

at9_status init_decoder(atrac9_handle* handle, unsigned char * config_data, int wlength);
at9_status init_config_data(ConfigData* config, unsigned char * config_data);
at9_status read_config_data(ConfigData* config);
at9_status init_frame(atrac9_handle* handle);
at9_status init_block(block* block, frame* parent_frame, int block_index);
at9_status init_channel(channel* channel, block* parent_block, int channel_index);