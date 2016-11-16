/*****************************************************************************
 * (c) Copyright 2010, Freescale Semiconductor Inc.
 * ALL RIGHTS RESERVED.
 ***************************************************************************//*!
 *
 * @file      audio_dma.h
 *
 * @author    N/A
 *
 * @version   1.0.1.0
 *
 * @date      Mar-22-2012
 *
 * @brief     header file for audio_dma.c, buffer size definitions
 *
 ******************************************************************************/

#ifndef _AUDIO_DMA_H
#define _AUDIO_DMA_H

#define AUDIO_SIZE   4096  // one channel audio , Buffers = 1024 16-bit words or 2048 bytes to read from SD card
#define SAMPLERATE 8000L // 8kHz sampling rate

#define PIT_IRQ2_ID PIT2_IRQn

#define USE_PWM 1
#define USE_I2S 2
#define USE_DAC 3

#define AUDIO_DMA_OUTPUT USE_DAC

typedef void (*tpfAudioDmaCb)( void );
// flags
extern volatile int Playing_Buff_A;

// audio buffers with samples
extern int16_t Audio_Source_Blk_A[AUDIO_SIZE], Audio_Source_Blk_B[AUDIO_SIZE];

/***************************************************************************
* API
****************************************************************************/
void Init_Audio(void);
void Start_Audio(void);
void restart_Audio(void);
void Stop_Audio(void);
void ClearAudioBuffers(void);
void ClearAudioBuf(int16_t *p);

void Init_FTM2(void);
void Init_PIT(void);
void Set_Audio_Playback_Dma(void);

void Set_VREF(void);
void Init_DAC(void);
void registerAudioCallback(tpfAudioDmaCb pfPingPongBufSwitchCb);


#endif //_AUDIO_DMA_H
