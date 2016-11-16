//
// Created by dothraki on 11/16/16.
//

#ifndef UBIRCH_FIRMWARE_MIC_H
#define UBIRCH_FIRMWARE_MIC_H

#ifdef __cplusplus
extern "C" {
#include "audio_dma.h"
#include "pdm2pcm.h"
#include <stdint.h>
}
#endif

#include <FatFs.h>

#define CIC2_R 8

typedef  int32_t CICREG;

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

typedef struct {
    uint8_t pcm_in_use;
    bool record2file;
    bool should_stop;
    bool stopped;
    int rec_fh;

    CICREG s2_sum1;
    CICREG s2_comb1_1;
    CICREG s2_comb1_2;
    CICREG s2_sum2;
    CICREG s2_comb2_1;
    CICREG s2_comb2_2;
    CICREG s2_sum3;
    CICREG s2_comb3_1;
    CICREG s2_comb3_2;

    uint8_t *pdm;
    uint8_t *pcm;
    uint8_t pcm_idx;
} mems_audio_t;

typedef struct {
    bool mems_dma_dry;
    uint8_t mems_cur_buf;

    uint8_t event_dma_rdy;
} audio_playback_t;


//initialize the  pin / port numbers for flexio

void audio_init(void);

void set_PDM_buf(uint8_t buf);

void set_PCM_buf(uint8_t buf);

void insert_PCM_buf(CICREG sample);

uint8_t get_cur_PCM_buf(void);

void process_audio();

void turn_recording_on(void);

void turn_recording_off(void);

void start_mic(void);

int stop_mic(void);

int should_stop_mic(void);

bool worker(uint8_t buf);

#endif //UBIRCH_FIRMWARE_MIC_H
