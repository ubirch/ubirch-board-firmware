//
// Created by dothraki on 11/16/16.
//

#include "ubirch/microphone/mic.h"
#include "ubirch/microphone/pdm2pcm.h"



int main (void)
{

  turn_recording_on();
  PRINTF("Recording starting\r\n");
  start_mic();
  uint32_t  start = millis(); // start the clock here to check how long it is been since started

  for (;;) {
    process_audio();

    if(!should_stop_mic()){
      if(millis() - start > 6000){
        stop_mic();
//        PRINTF("Recording stopped");
      }
    }

  }
}
