//
// Created by dothraki on 10/24/16.
//

#ifndef UBIRCH_FIRMWARE_TOSDCARD_H
#define UBIRCH_FIRMWARE_TOSDCARD_H

void init_sdhc_pins();
int pcm__sdhc(uint8_t *g_bufferWrite);

#endif //UBIRCH_FIRMWARE_TOSDCARD_H
