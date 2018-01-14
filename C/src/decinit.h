#pragma once

#include "error_codes.h"
#include "structures.h"

at9_status init_decoder(atrac9_handle* handle, unsigned char * config_data, int wlength);
