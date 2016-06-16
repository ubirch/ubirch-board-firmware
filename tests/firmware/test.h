#ifndef __UBIRCH_FIRMWARE_TEST__
#define __UBIRCH_FIRMWARE_TEST__

#ifdef __cplusplus
extern "C" {
#endif

uint32_t test_1ms_ticker = 0;

#define TEST(prefix, test) PRINTF("%-15s: %s\r\n", prefix, ((test == 0) ? "OK" : "FAIL"));

#ifdef __cplusplus
}
#endif

#endif
