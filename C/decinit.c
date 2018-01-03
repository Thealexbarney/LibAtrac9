#include "bit_reader.h"
#include "decinit.h"
#include "error_codes.h"
#include "structures.h"
#include "tables.h"
#include <string.h>

static int BlockTypeToChannelCount(BlockType block_type);

at9_status init_decoder(atrac9_handle* handle, unsigned char* config_data, int wlength)
{
	ERROR_CHECK(init_config_data(&handle->config, config_data));
	ERROR_CHECK(init_frame(handle));
	handle->wlength = wlength;
	handle->initialized = 1;
	return ERR_SUCCESS;
}

at9_status init_config_data(ConfigData* config, unsigned char* config_data)
{
	memcpy(config->ConfigData, config_data, CONFIG_DATA_SIZE);
	ERROR_CHECK(read_config_data(config));

	config->FramesPerSuperframe = 1 << config->SuperframeIndex;
	config->SuperframeBytes = config->FrameBytes << config->SuperframeIndex;

	config->ChannelConfig = ChannelConfigs[config->ChannelConfigIndex];
	config->ChannelCount = config->ChannelConfig.ChannelCount;
	config->SampleRate = SampleRates[config->SampleRateIndex];
	config->HighSampleRate = config->SampleRateIndex > 7;
	config->FrameSamplesPower = SamplingRateIndexToFrameSamplesPower[config->SampleRateIndex];
	config->FrameSamples = 1 << config->FrameSamplesPower;
	config->SuperframeSamples = config->FrameSamples * config->FramesPerSuperframe;

	return ERR_SUCCESS;
}

at9_status read_config_data(ConfigData* config)
{
	bit_reader_cxt br;
	init_bit_reader_cxt(&br, &config->ConfigData);

	const int header = read_int(&br, 8);
	config->SampleRateIndex = read_int(&br, 4);
	config->ChannelConfigIndex = read_int(&br, 3);
	const int validation_bit = read_int(&br, 1);
	config->FrameBytes = read_int(&br, 11) + 1;
	config->SuperframeIndex = read_int(&br, 2);

	if (header != 0xFE || validation_bit != 0)
	{
		return ERR_BAD_CONFIG_DATA;
	}

	return ERR_SUCCESS;
}

at9_status init_frame(atrac9_handle* handle)
{
	const int block_count = handle->config.ChannelConfig.BlockCount;
	handle->frame.config = &handle->config;

	for (int i = 0; i < block_count; i++)
	{
		ERROR_CHECK(init_block(&handle->frame.Blocks[i], &handle->frame, i));
	}

	return ERR_SUCCESS;
}

at9_status init_block(block* block, frame* parent_frame, int block_index)
{
	block->Frame = parent_frame;
	block->BlockIndex = block_index;
	block->config = parent_frame->config;
	block->BlockType = block->config->ChannelConfig.Types[block_index];
	block->ChannelCount = BlockTypeToChannelCount(block->BlockType);

	for (int i = 0; i < block->ChannelCount; i++)
	{
		ERROR_CHECK(init_channel(&block->Channels[i], block, i));
	}

	return ERR_SUCCESS;
}

at9_status init_channel(channel* channel, block* parent_block, int channel_index)
{
	channel->Block = parent_block;
	channel->Frame = parent_block->Frame;
	channel->config = parent_block->config;
	channel->ChannelIndex = channel_index;
	channel->mdct.bits = parent_block->config->FrameSamplesPower;
	return ERR_SUCCESS;
}

static int BlockTypeToChannelCount(BlockType block_type)
{
	switch (block_type)
	{
	case Mono:
		return 1;
	case Stereo:
		return 2;
	case LFE:
		return 1;
	default:
		return 0;
	}
}
