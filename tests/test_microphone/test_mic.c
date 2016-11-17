//
// Created by dothraki on 11/16/16.
//

#include <fsl_debug_console.h>
#include <board.h>
#include <ubirch/timer.h>
#include "ubirch/microphone/mic.h"
#include "ubirch/microphone/pdm2pcm.h"

volatile uint32_t milliseconds = 0;
bool on = true;

void SysTick_Handler() {
  milliseconds++;
  if (milliseconds % 1000 == 0) on = !on;
  BOARD_LED0(on);
}

int main (void)
{
  board_init();
  board_console_init(BOARD_DEBUG_BAUD);
  SysTick_Config(BOARD_SYSTICK_100MS / 10);

  audio_init();
  turn_recording_on();
  PRINTF("Recording starting\r\n");
  start_mic();
  uint32_t  start = millis(); // start the clock here to check how long it is been since started

  for (;;) {
    process_audio();

    if(!should_stop_mic()){
      if(millis() - start > 6000){
        stop_mic();
        PRINTF("Recording stopped");
      }
    }

  }
}
