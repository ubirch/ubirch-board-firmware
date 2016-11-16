//
// Created by dothraki on 11/16/16.
//

#include "mic.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "Serial.h"

#include "wav_record.h"


static int count;

mems_audio_t mems_audio;
audio_playback_t audio_playback;

void audio_init(void){
  FLEXIO_MemMicInit();
}


void set_PDM_buf(uint8_t buf){
  mems_audio.pdm = &PDMRxData[buf][0];
}

void set_PCM_buf(uint8_t buf){
  mems_audio.pcm = &PCMData[buf][0];
}

// write little endian to wav file
void insert_PCM_buf(CICREG sample){
  mems_audio.pcm++ = sample & 0xFF;
  mems_audio.pcm++ = ((sample >> 8) & 0xFF);
}

uint8_t get_cur_PCM_buf(void){
  return mems_audio.pcm_in_use;
}


void process_audio(){
  if(audio_playback.mems_dma_rdy){
    count++;
    worker(audio_playback.mems_cur_buf);
    audio_playback.mems_dma_rdy = false;
    if(mems_audio.stopped){
      // that last worker task was the last one
      turnRecordingOff();
    }
  }
  if(audio_playback.event_dma_rdy == 1){
    audio_playback.event_dma_rdy = 0;
    pingPongManager();
  }
}

void turn_recording_on(void){
  mems_audio.rec_fh = file.open("record.raw", FA_WRITE | FA_CREATE_ALWAYS);
  if (mems_audio.rec_fh >= 0){
    PRINTF("Recording to file new");
    mems_audio.record2file = true;
  }
}

void turn_recording_off(void){
  if(mems_audio.record2file){
    file.close(mems_audio.rec_fh);
    PRINTF("Close record file\r\n");

    mems_audio.rec_fh = 1; //set handler to the close state
    mems_audio.record2file = false;
  }
}

void start_mic(void){
  mems_audio.should_stop = false;
  mems_audio.stopped = false;
  Mic_Start();
}

int stop_mic(void){
  mems_audio.should_stop = true;
  PRINTF("Number of frames acquired %d\r\n", count);
}

int should_stop_mic(void){
  return mems_audio.should_stop;
}

// this converts the pdm signal into PCM
bool worker(uint8_t buf){

  CICREG Rout2;
  CICREG stage1, stage2, stage3;
  static int pcm;
  static int pcm0, pcm1;

  set_PDM_buf(buf);					// the ready PDM buffer
  set_PCM_buf(mems_audio.pcm_in_use);

  for(uint8_t k=0; k < PCM_FRAME; k++){
    // extract the PDM samples taken care of the reorder made by using FlexIO and DMA
    for(uint8_t j=0; j < CIC2_R/2; j++) {
      mems_audio.s2_sum1 += pdmsum8[pdm_[((CIC2_R/2)-1)-j]];
      mems_audio.s2_sum2 += mems_audio.s2_sum1;
      mems_audio.s2_sum3 += mems_audio.s2_sum2;
    }
    mems_audio.pdm += 4;
    for(uint8_t j=0; j < CIC2_R/2; j++) {
      mems_audio.s2_sum1 += pdmsum8[mems_audio.pdm[((CIC2_R/2)-1)-j]];
      mems_audio.s2_sum2 += mems_audio.s2_sum1;
      mems_audio.s2_sum3 += mems_audio.s2_sum2;
    }
    mems_audio.pdm += 4;					// advance to next cycle

    Rout2 = mems_audio.s2_sum3;

    stage1 = Rout2 - mems_audio.s2_comb1_2;
    mems_audio.s2_comb1_2 = mems_audio.s2_comb1_1;
    mems_audio.s2_comb1_1 = Rout2;

    stage2 = stage1 - mems_audio.s2_comb2_2;
    mems_audio.s2_comb2_2 = mems_audio.s2_comb2_1;
    mems_audio.s2_comb2_1 = stage1;

    stage3 = stage2 - mems_audio.s2_comb3_2;
    mems_audio.s2_comb3_2 = mems_audio.s2_comb3_1;
    mems_audio.s2_comb3_1 = stage2;

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
  if(mems_audio.pcm_in_use == 0){
    mems_audio.pcm_in_use = 1;
  }else{
    mems_audio.pcm_in_use = 0;
  }
  return true;
}
