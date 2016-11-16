#ifndef __PDM2PCM_H__
#define __PDM2PCM_H__

#include <stdint.h>

#define PDM_TRANSFER_SIZE 1280U            // 20 ms of PDM data at 512 Kbps that will down convert to 160 words per "frame" at 8000 Hz (64 DS factor)
#define PCM_FRAME	(PDM_TRANSFER_SIZE/8)				// 16 bit word width

extern uint8_t PDMRxData[2][PDM_TRANSFER_SIZE];

typedef int (*tpfMemsFrameReadyCb)(uint8_t bufferIndex );

void FLEXIO_MemMicInit(void);
void Mic_Start(void);
void Mic_Stop(void);
void registerRecordingDeviceCb(tpfMemsFrameReadyCb readyCb);

#endif
