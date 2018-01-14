#pragma once

#include "bit_reader.h"
#include "error_codes.h"
#include "structures.h"

at9_status read_scale_factors(channel* channel, bit_reader_cxt* br);
