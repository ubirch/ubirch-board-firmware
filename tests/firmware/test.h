#ifndef __UBIRCH_FIRMWARE_TEST__
#define __UBIRCH_FIRMWARE_TEST__

uint32_t test_1ms_ticker = 0;

#define TEST(prefix, test) PRINTF("%-15s: %s\r\n", prefix, ((test == 0) ? "OK" : "FAIL"));

#endif
