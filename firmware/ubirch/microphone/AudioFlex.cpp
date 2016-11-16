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

#include "AudioFlex.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "Serial.h"

#include "wav_record.h"

extern FileFs file;
extern SerialConsole Serial;

volatile unsigned int dataEnd;
//volatile byte buffer[2][buffSize], buffCount = 0;
//volatile boolean buffEmpty[2] = {true,true}, whichBuff = false, playing = 0, a, b;
////*** Options/Indicators from MSb to LSb: paused, qual, rampUp, 2-byte samples, loop, loop2nd track, 16-bit ***
//byte optionByte = 0b01100000;

#define debug	1

#define VOLUME_CTRL 1

#if debug
	static int count = 0;
#endif

#define PCM_EXT_RAW	".raw"
//#define HANDLE_TAGS

uint8_t AudioFlexClass::event_dma_rdy_ = 0;
uint16_t AudioFlexClass::buf_pos_ = 0;
bool AudioFlexClass::reload_ = false;
bool AudioFlexClass::last_buf_ = false;
bool AudioFlexClass::stop_ = false;
bool AudioFlexClass::reload_mode_ = false;
bool AudioFlexClass::playing_file_ = false;
int AudioFlexClass::wav_hlr_ = -1;
bool AudioFlexClass::playing_ = false;
// MEMS variables
bool AudioFlexClass::mems_dma_rdy_ = false;
uint8_t AudioFlexClass::mems_cur_buf_ = 0;
bool MemsCIC::record2file_ = false;
bool MemsCIC::should_stop_ = false;
bool MemsCIC::stopped_ = true;
int MemsCIC::rec_fh_ = -1;

unsigned char __attribute__((aligned(32), section(".myRAM"))) file_buf[2*AUDIO_SIZE];
uint8_t __attribute__((aligned(32), section(".myRAM"))) PCMData[2][2*PCM_FRAME];

static void dma_audio_event_cb(void){
	AudioFlex.audio_event_cb();
//	AudioFlex.pingPongManager();
}

static int dma_record_event_cb(uint8_t rdy_buf){
	AudioFlex.mems_event_cb(rdy_buf);
	return (AudioFlex.shouldStop());
}


AudioFlexClass::AudioFlexClass(){
	registerAudioCallback(dma_audio_event_cb);						// Register the Playback device (DAC)
	registerRecordingDeviceCb(dma_record_event_cb);					// Register the recording Device (MEMS)
}

volatile void AudioFlexClass::audio_event_cb(void){
	event_dma_rdy_ = 1;
}

volatile void AudioFlexClass::mems_event_cb(uint8_t buf){
	mems_dma_rdy_ = true;
	mems_cur_buf_ = buf;
	if(should_stop_){
		stopped_ = true;				// signal we can now close handlers such a file
	}
}

volatile void AudioFlexClass::pingPongManager(void){
	signed short * pSample;
	signed short * p_buf;
	uint16_t size;
#if debug
	static int count = 0;
#endif

	if(playing_){
		if(stop_){
			stop_ = false;
			file.close(wav_hlr_);
			wav_hlr_ = -1;
			Stop_Audio();
			return;
		}
		p_buf = (Playing_Buff_A == 1) ? &Audio_Source_Blk_B[0] : &Audio_Source_Blk_A[0]; // if DMA0 reads buff A, reload B by CPU otherwise reload A by CPU
		if(file.isOpen(wav_hlr_)){
			if((size = file.read(wav_hlr_, file_buf, sizeof(file_buf))) < sizeof(file_buf)){
				// this is the last buffer of file
				ClearAudioBuf(p_buf);			// clear so we end with Mute data...
				stop_ = true;
#if debug
				Serial.print("Playback comes to an end, count = ");
				Serial.println(count);
				count = 0;
#endif
			}
		}else{
			wav_hlr_ = -1;
#if debug
			Serial.print("[Error] : File was closed, count = ");
			Serial.println(count);
			count = 0;
#endif
			Stop_Audio();
			stop_ = true;
			return;
		}
		pSample = (signed short *) file_buf;			// Here we can do some logic to gather the correct buffer stream or adjust the Volume
		for(uint16_t i=0; i < size/2; i++)
		{
		  *p_buf = (unsigned short)(((pSample[i])+32768))>>4;
#if VOLUME_CTRL
		  *p_buf >>= 1;
#endif
		  p_buf++;
		}
#if debug
		count ++;
#endif
	}
}


bool AudioFlexClass::wavInfo(char* filename){

  //check for the string WAVE starting at 8th bit in header of file
	wav_hlr_ = file.open(filename, FA_READ);

	if( wav_hlr_ < 0 ){ return 0; }
	file.seekSet(wav_hlr_, 8);

	char tmp[4];
	char wavStr[] = {'W','A','V','E'};
	file.read(wav_hlr_, tmp, 4);
	if(strncmp(tmp, wavStr, 4) != 0){
#if defined (debug)
		Serial.println("WAV ERROR");
#endif
		return 0;
	}

	#if defined (STEREO_OR_16BIT)
		byte stereo, bps;
		file.seekSet(wav_hlr_, 22);
		stereo = file.readChar();
		file.seekSet(wav_hlr_, 24);
	#else
		file.seekSet(wav_hlr_, 24);
	#endif

	SAMPLE_RATE = file.readChar(wav_hlr_);
	SAMPLE_RATE = file.readChar(wav_hlr_) << 8 | SAMPLE_RATE;


	// need to port this functionality
//	#if defined (USE_TIMER2)
//
//		if(SAMPLE_RATE < 9000 ){ SR = 0; }
//		else if(SAMPLE_RATE < 20000){ SR = 1; }
//		else if(SAMPLE_RATE < 28000){ SR = 2; }
//		else{ SR = 3; }
//	#endif
//	#if defined (STEREO_OR_16BIT)
//		//verify that Bits Per Sample is 8 (0-255)
//		seek(34);
//		bps = sFile.read();
//		bps = sFile.read() << 8 | bps;
//		if( stereo == 2){ //_2bytes=1;
//			bitSet(optionByte,4);
//		}else
//		if( bps == 16 ){
//			bitSet(optionByte,1);
//			bitSet(optionByte,4);
//		}else{ bitClear(optionByte,4); bitClear(optionByte,1);}
//	#endif

	#if defined (HANDLE_TAGS)

		file.seekSet(wav_hlr_, 36);
		char datStr[4] = {'d','a','t','a'};
		for (byte i =0; i<4; i++){
			if(file.readChar(wav_hlr_) != datStr[i]){
				file.seekSet(wav_hlr_, 40);
				unsigned int siz = file.readChar(wav_hlr_);
				siz = (file.readChar(wav_hlr_) << 8 | siz)+2;
				file.seekSet(wav_hlr_, file.curPosition(wav_hlr_) + siz);
				for (byte i=0; i<4; i++){
					if(file.readChar(wav_hlr_) != datStr[i]){
						return 0;
					}
				}
			}
		 }

		unsigned long dataBytes = file.readChar(wav_hlr_);
		for (byte i =8; i<32; i+=8){
			dataBytes = file.readChar(wav_hlr_) << i | dataBytes;
		}
		dataEnd = file.fileSize(wav_hlr_) - file.curPosition(wav_hlr_) - dataBytes + buffSize;

	#else //No Tag handling

		file.seekSet(wav_hlr_, 44);
		dataEnd = AUDIO_SIZE;

	#endif

	return 1;

}



void AudioFlexClass::begin() {
	/* Modules configuration */
#if PLAYBACK_EN
	Init_Audio();
#endif
#if MEMS_TEST
	FLEXIO_MemMicInit();
#endif
}


void AudioFlexClass::stopPlayback(){
	if(file.isOpen(wav_hlr_)){
		file.close(wav_hlr_);
	}
}

void AudioFlexClass::pause(){

}

void AudioFlexClass::end() {

}

void AudioFlexClass::process(){
	if(mems_dma_rdy_){
		count++;
		worker(mems_cur_buf_);
		mems_dma_rdy_ = false;
		if(stopped_){
			// that last worker task was the last one
			turnRecordingOff();
		}
	}
	if(event_dma_rdy_ == 1){
		event_dma_rdy_ = 0;
		pingPongManager();
	}
}

/*void AudioFlexClass::prepare(int volume){
//Not Implemented yet
}*/
//void AudioFlexClass::play(void){
//	reload_mode_ = false;
//	Start_Audio();
//}AudioFlex


// play from file, i.e
//	AudioFlex.play("sine_1000Hz.raw");
//	AudioFlex.play("moh_8sec.raw");

void AudioFlexClass::play(char *filename, unsigned long seekPoint) {

	stopPlayback();
	boolean raw_match = !strncmp(filename, PCM_EXT_RAW, strlen(PCM_EXT_RAW));
	if(!raw_match){
		// we are opening a RAW PCM file
		wav_hlr_ = file.open(filename, FA_READ);
		if( wav_hlr_ < 0 ){ return; }
		SAMPLE_RATE = 8000;
	}else{
		if(!wavInfo(filename)){
			#if defined (debug)
				Serial.println("WAV ERROR");
			#endif
			return;
		}	//verify its a valid wav file
	}

	if(seekPoint > 0){
		seekPoint = (SAMPLE_RATE*seekPoint) + file.curPosition(wav_hlr_);
		file.seekSet(wav_hlr_, seekPoint);	//skip the header info
	}

//	playing = 1; bitClear(optionByte,7); //paused = 0;

	if(SAMPLE_RATE > 45050 ){ SAMPLE_RATE = 24000;
	#if defined (debug)
  	  	Serial.print("SAMPLE RATE TOO HIGH: ");
  	  	Serial.println(SAMPLE_RATE);
  	#endif
  	}
	reload_mode_ = false;
	wav_record_file = &file_buf[0];
	playing_ = true;
	Start_Audio();
}


/**
 * Configures the DAC in event triggered mode.
 *
 * Configures the DAC to use the module's default configuration, with output
 * channel mode configured for event triggered conversions.
 */
void AudioFlexClass::dacConfigure(void){

}

/**
 * Configures the TC to generate output events at the sample frequency.
 *
 * Configures the TC in Frequency Generation mode, with an event output once
 * each time the audio sample frequency period expires.
 */
 void AudioFlexClass::tcConfigure(uint32_t sampleRate)
{

}

bool AudioFlexClass::tcIsSyncing()
{
	return false;
}

void AudioFlexClass::tcStartCounter()
{
  // Enable TC

}

void AudioFlexClass::tcReset()
{

}

void AudioFlexClass::tcDisable()
{

}

AudioFlexClass AudioFlex;



/***********************************/


MemsCIC::MemsCIC()
	:pcm_in_use_(0),
	s2_sum1(0),
	s2_comb1_1(0),
	s2_comb1_2(0),
	s2_sum2(0),
	s2_comb2_1(0),
	s2_comb2_2(0),
	s2_sum3(0),
	s2_comb3_1(0),
	s2_comb3_2(0),
	pdm_(&PDMRxData[0][0]),
	pcm_(&PCMData[0][0])
{

}

void MemsCIC::setPDMbuf(uint8_t buf){
//	assert(buf < 2);
	pdm_ = &PDMRxData[buf][0];
}

void MemsCIC::setPCMbuf(uint8_t buf){
//	assert(buf < 2);
	pcm_ = &PCMData[buf][0];
}

void MemsCIC::insertPCMbuf(CICREG sample){	// this writes little endian to wav file i.e.
	*pcm_++ = sample & 0xFF;
	*pcm_++ = ((sample >> 8) & 0xFF);
}

uint8_t MemsCIC::getCurPCMbuf(void){
	return pcm_in_use_;
}

void MemsCIC::turnRecordingOn(void){
	rec_fh_ = file.open("record.raw", FA_WRITE | FA_CREATE_ALWAYS);
	if(rec_fh_ >= 0){
#if debug
		Serial.println("Mic record to file NEW");
#endif
		record2file_ = true;
	}
}

void MemsCIC::turnRecordingOff(void){
	if(record2file_){
		file.close(rec_fh_);
#if debug
		Serial.println("closing record file");
#endif
		rec_fh_ = -1;			// set handler to the close state
		record2file_ = false;
	}
}

void MemsCIC::startMic(void){
	should_stop_ = false;
	stopped_ = false;
	Mic_Start();
}

void MemsCIC::stopMic(void){
	should_stop_ = true;
	Serial.print("[MEMS MIC] Number of Frames acquired = ");
	Serial.println(count);
}

int MemsCIC::shouldStop(void){
	return should_stop_;			// this variable is gathered from lower layer callback to stop there (DMA callback)
}

bool MemsCIC::worker(uint8_t buf){

	CICREG Rout2;
	CICREG stage1, stage2, stage3;
	static int pcm;
	static int pcm0, pcm1;

	setPDMbuf(buf);					// the ready PDM buffer
	setPCMbuf(pcm_in_use_);
	for(uint8_t k=0; k < PCM_FRAME; k++){
		// extract the PDM samples taken care of the reorder made by using FlexIO and DMA
		for(uint8_t j=0; j < CIC2_R/2; j++) {
			s2_sum1 += pdmsum8[pdm_[((CIC2_R/2)-1)-j]];
			s2_sum2 += s2_sum1;
			s2_sum3 += s2_sum2;
		}
		pdm_ += 4;
		for(uint8_t j=0; j < CIC2_R/2; j++) {
			s2_sum1 += pdmsum8[pdm_[((CIC2_R/2)-1)-j]];
			s2_sum2 += s2_sum1;
			s2_sum3 += s2_sum2;
		}
		pdm_ += 4;					// advance to next cycle

		Rout2 = s2_sum3;

		stage1 = Rout2 - s2_comb1_2;
		s2_comb1_2 = s2_comb1_1;
		s2_comb1_1 = Rout2;

		stage2 = stage1 - s2_comb2_2;
		s2_comb2_2 = s2_comb2_1;
		s2_comb2_1 = stage1;

		stage3 = stage2 - s2_comb3_2;
		s2_comb3_2 = s2_comb3_1;
		s2_comb3_1 = stage2;

    	// High pass filter to remove DC, roll-off at alpha=63/64 and 7812.5 Hz
    	// implies RC=16.256 ms, or Fc=9.79 Hz

    	// alpha at 7812.5 Hz        Fcut
    	//   255/256    32.64 ms      4.876 Hz
    	//   127/128    16.256 ms     9.79 Hz
    	//    63/64      8.064 ms    19.74 Hz
    	//    31/32      3.968 ms    40.1 Hz

		pcm = stage3;
    	pcm1 = 63 * (pcm1 + pcm - pcm0) / 64;
    	pcm0 = pcm;
    	pcm = pcm1;

    	pcm << 1;			// increase volume simple test / not very successful
		// queue the finished PCM filtered sample
		insertPCMbuf(pcm);

	}

	if(record2file_){
		if(file.isOpen(rec_fh_)){
			file.write(rec_fh_, &PCMData[pcm_in_use_][0], 2*PCM_FRAME);			// PCMData is of int16_t type
			// perhaps need to check for an error
		}
	}
	// swap PCM buffers - Ping Pong
	if(pcm_in_use_ == 0){
		pcm_in_use_ = 1;
	}else{
		pcm_in_use_ = 0;
	}
	return true;
}

