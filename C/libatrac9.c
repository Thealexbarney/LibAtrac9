#include "libatrac9.h"
#include "structures.h"
#include <stdlib.h>
#include <string.h>
#include "decinit.h"
#include "decoder.h"

HANDLE_ATRAC9 GetHandle()
{
	struct atrac9_handle* handle = malloc(sizeof(atrac9_handle));
	memset(handle, 0, sizeof(atrac9_handle));
	return handle;
}

void ReleaseHandle(HANDLE_ATRAC9 handle)
{
	free(handle);
}

int DecInit(HANDLE_ATRAC9 handle, unsigned char * pConfigData, int wlength)
{
	if (wlength != 16)
	{
		return -1;
	}

	ERROR_CHECK(init_decoder(handle, pConfigData, wlength));

	return 0;
}

int DecDecode(HANDLE_ATRAC9 handle, const unsigned char * pStreamBuffer, int * pNByteUsed, void * pPcmBuffer)
{
	at9_status status = Decode(handle, pStreamBuffer, pPcmBuffer, pNByteUsed);
	return status;
}

int GetCodecInfo(HANDLE_ATRAC9 handle, CodecInfo * pCodecInfo)
{
	atrac9_handle* h = handle;

	pCodecInfo->channels = h->config.ChannelCount;
	pCodecInfo->channelConfigIndex = h->config.ChannelConfigIndex;
	pCodecInfo->samplingRate = h->config.SampleRate;
	pCodecInfo->superframeSize = h->config.SuperframeBytes;
	pCodecInfo->framesInSuperframe = h->config.FramesPerSuperframe;
	pCodecInfo->frameSamples = h->config.FrameSamples;
	pCodecInfo->wlength = h->wlength;
	memcpy(pCodecInfo->configData, h->config.ConfigData, CONFIG_DATA_SIZE);
	return ERR_SUCCESS;
}
