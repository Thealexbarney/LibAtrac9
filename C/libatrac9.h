#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#ifdef COMPILING_DLL 
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)  
#endif

typedef void* HANDLE_ATRAC9;

typedef struct {
	int channels;
	int channelConfigIndex;
	int samplingRate;
	int superframeSize;
	int framesInSuperframe;
	int frameSamples;
	int wlength;
	unsigned char configData[4];
} CodecInfo;

DLLEXPORT HANDLE_ATRAC9 GetHandle(void);
DLLEXPORT void ReleaseHandle(HANDLE_ATRAC9 handle);

DLLEXPORT int DecInit(HANDLE_ATRAC9 handle, unsigned char *pConfigData, int wlength);
DLLEXPORT int DecDecode(HANDLE_ATRAC9 handle, const unsigned char *pStreamBuffer, int *pNByteUsed, void *pPcmBuffer);
DLLEXPORT int GetCodecInfo(HANDLE_ATRAC9 handle, CodecInfo *pCodecInfo);

#ifdef __cplusplus
}
#endif
