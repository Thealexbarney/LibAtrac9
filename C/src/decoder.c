#include "decoder.h"
#include "band_extension.h"
#include "bit_reader.h"
#include "imdct.h"
#include "quantization.h"
#include "tables.h"
#include "unpack.h"
#include "utility.h"
#include <math.h>
#include <string.h>

static At9Status DecodeFrame(Frame* frame, BitReaderCxt* br);
static void ImdctBlock(Block* block);
static void ApplyIntensityStereo(Block* block);
static void PcmFloatToShort(Frame* frame, short* pcmOut);

At9Status Decode(Atrac9Handle* handle, const unsigned char* audio, unsigned char* pcm, int* bytesUsed)
{
	handle->Frame.FrameNum++;
	BitReaderCxt br;
	InitBitReaderCxt(&br, audio);
	ERROR_CHECK(DecodeFrame(&handle->Frame, &br));

	PcmFloatToShort(&handle->Frame, (short*)pcm);

	*bytesUsed = br.Position / 8;
	return ERR_SUCCESS;
}

static At9Status DecodeFrame(Frame* frame, BitReaderCxt* br)
{
	ERROR_CHECK(UnpackFrame(frame, br));

	for (int i = 0; i < frame->Config->ChannelConfig.BlockCount; i++)
	{
		Block* block = &frame->Blocks[i];

		DequantizeSpectra(block);
		ApplyIntensityStereo(block);
		ScaleSpectrumBlock(block);
		ApplyBandExtension(block);
		ImdctBlock(block);
	}

	return ERR_SUCCESS;
}

static void PcmFloatToShort(Frame* frame, short* pcmOut)
{
	const int endSample = frame->Config->FrameSamples;
	short* dest = pcmOut;
	for (int d = 0, s = 0; s < endSample; d++, s++)
	{
		for (int i = 0; i < frame->Config->ChannelConfig.BlockCount; i++)
		{
			Block* block = &frame->Blocks[i];

			for (int j = 0; j < block->ChannelCount; j++)
			{
				Channel* channel = &block->Channels[j];
				double* pcmSrc = channel->Pcm;

				const double sample = pcmSrc[d];
				const int roundedSample = (int)floor(sample + 0.5);
				*dest++ = Clamp16(roundedSample);
			}
		}
	}
}

static void ImdctBlock(Block* block)
{
	for (int i = 0; i < block->ChannelCount; i++)
	{
		Channel* channel = &block->Channels[i];

		RunImdct(&channel->Mdct, channel->Spectra, channel->Pcm);
	}
}

static void ApplyIntensityStereo(Block* block)
{
	if (block->BlockType != Stereo) return;

	const int totalUnits = block->QuantizationUnitCount;
	const int stereoUnits = block->StereoQuantizationUnit;
	if (stereoUnits >= totalUnits) return;

	Channel* source = &block->Channels[block->PrimaryChannelIndex == 0 ? 0 : 1];
	Channel* dest = &block->Channels[block->PrimaryChannelIndex == 0 ? 1 : 0];

	for (int i = stereoUnits; i < totalUnits; i++)
	{
		const int sign = block->JointStereoSigns[i];
		for (int sb = QuantUnitToCoeffIndex[i]; sb < QuantUnitToCoeffIndex[i + 1]; sb++)
		{
			if (sign > 0)
			{
				dest->Spectra[sb] = -source->Spectra[sb];
			}
			else
			{
				dest->Spectra[sb] = source->Spectra[sb];
			}
		}
	}
}

int GetCodecInfo(Atrac9Handle* handle, CodecInfo * pCodecInfo)
{
	pCodecInfo->Channels = handle->Config.ChannelCount;
	pCodecInfo->ChannelConfigIndex = handle->Config.ChannelConfigIndex;
	pCodecInfo->SamplingRate = handle->Config.SampleRate;
	pCodecInfo->SuperframeSize = handle->Config.SuperframeBytes;
	pCodecInfo->FramesInSuperframe = handle->Config.FramesPerSuperframe;
	pCodecInfo->FrameSamples = handle->Config.FrameSamples;
	pCodecInfo->Wlength = handle->Wlength;
	memcpy(pCodecInfo->ConfigData, handle->Config.ConfigData, CONFIG_DATA_SIZE);
	return ERR_SUCCESS;
}
