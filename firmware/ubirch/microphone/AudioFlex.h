/*
 * Copyright (c) 2016 by
 Manuel Iglesias <mhanuel@ieee.org>


 * Audio library for Flexduino NXP K82F (based on TMRpcm libraries + a little from AudioZero  and a lot of new stuff for Kinetics ARCH)
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef AUDIOZERO_H
#define AUDIOZERO_H

#include "Arduino.h"
#include "Print.h"
extern "C" {
#include "audio_dma.h"
#include "pdm2pcm.h"
}
#include <FatFs.h>

#define PLAYBACK_EN		1
#define MEMS_TEST		0				// useful to disable or enable at this stage that we are still testing and debugging library

/*  CIC Filter Class for MEMS PDM data to PCM */

/* CIC filter state */
#define CIC2_R 8
typedef int32_t CICREG;

const int8_t pdmsum8[256] = {
#   define S(n) (2*(n)-8)
#   define B2(n) S(n),  S(n+1),  S(n+1),  S(n+2)
#   define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#   define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
		B6(0), B6(1), B6(1), B6(2)
#undef S
#undef B6
#undef B4
#undef B2
};


class MemsCIC{
public:
	MemsCIC();
	bool worker(uint8_t buf);
	void setPDMbuf(uint8_t buf);
	void insertPCMbuf(CICREG sample);
	void setPCMbuf(uint8_t buf);
	uint8_t getCurPCMbuf(void);
	void turnRecordingOn(void);
	void turnRecordingOff(void);
	void startMic(void);
	void stopMic(void);
	int shouldStop(void);

protected:
	uint8_t pcm_in_use_;
	static bool record2file_;
	static bool should_stop_;
	static int rec_fh_;
	static bool stopped_;
private:
	CICREG s2_sum1;
	CICREG s2_comb1_1;
	CICREG s2_comb1_2;
	CICREG s2_sum2;
	CICREG s2_comb2_1;
	CICREG s2_comb2_2;
	CICREG s2_sum3;
	CICREG s2_comb3_1;
	CICREG s2_comb3_2;
	uint8_t *pdm_;
	uint8_t *pcm_;
	uint8_t pcm_idx_;
};


/*   Audio Class for Playback and Recording */


class AudioFlexClass: public MemsCIC{
public:

	AudioFlexClass();
	void begin();
	bool wavInfo(char* filename);
//	void prepare(int volume); //not implemented yet
//	void play();
	void play(char *filename) { play(filename, 0); };
	void play(char *filename, unsigned long seekPoint);
	void stopPlayback();
	void pause();
	void end();
	volatile void audio_event_cb(void);
	volatile void mems_event_cb(uint8_t buf);
	volatile void pingPongManager(void);
	void process();

	unsigned int SAMPLE_RATE;
	unsigned long wav_record_size;
	unsigned char * wav_record_file;


private:
	void dacConfigure(void);
	void tcConfigure(uint32_t sampleRate);
	bool tcIsSyncing(void);
	void tcStartCounter(void);
	void tcReset(void);
	void tcEnable(void);
	void tcDisable(void);

	static bool mems_dma_rdy_;
	static uint8_t mems_cur_buf_;
	// DAC playback
	static uint8_t event_dma_rdy_;
	static uint16_t buf_pos_;
	static bool reload_;
	static bool last_buf_;
	static bool stop_;
	static bool reload_mode_;
	static bool playing_file_;
	static int wav_hlr_;
	static bool playing_;
};

extern AudioFlexClass AudioFlex;

#endif
