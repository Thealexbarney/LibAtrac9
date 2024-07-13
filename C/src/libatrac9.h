#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define LIBATRAC9_DLLEXPORT __declspec(dllexport)
#define LIBATRAC9_DLLIMPORT __declspec(dllimport)
#else
#define LIBATRAC9_DLLEXPORT
#define LIBATRAC9_DLLIMPORT
#endif

#if defined(LIBATRAC9_STATIC_DEFINE)
#define LIBATRAC9_EXPORT
#elif defined(COMPILING_DLL)
#define LIBATRAC9_EXPORT LIBATRAC9_DLLEXPORT
#else
#define LIBATRAC9_EXPORT LIBATRAC9_DLLIMPORT
#endif

#define ATRAC9_CONFIG_DATA_SIZE 4

typedef struct {
	int channels;
	int channelConfigIndex;
	int samplingRate;
	int superframeSize;
	int framesInSuperframe;
	int frameSamples;
	int wlength;
	unsigned char configData[ATRAC9_CONFIG_DATA_SIZE];
} Atrac9CodecInfo;

LIBATRAC9_EXPORT void* Atrac9GetHandle(void);
LIBATRAC9_EXPORT void Atrac9ReleaseHandle(void* handle);

LIBATRAC9_EXPORT int Atrac9InitDecoder(void* handle, unsigned char *pConfigData);
LIBATRAC9_EXPORT int Atrac9Decode(void* handle, const unsigned char *pAtrac9Buffer, short *pPcmBuffer, int *pNBytesUsed);

LIBATRAC9_EXPORT int Atrac9GetCodecInfo(void* handle, Atrac9CodecInfo *pCodecInfo);

#ifdef __cplusplus
}
#endif
