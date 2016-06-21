
#ifndef __UBIRCH_FIRMWARE_TEST__
#define __UBIRCH_FIRMWARE_TEST__

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t test_1ms_ticker;

void failed(char *file, int line, char *expr);

#define TEST(prefix, test) PRINTF("= %-15s: %s\r\n", prefix, ((test == 0) ? "OK" : "FAIL"))

// either continue or fail with a message
#define __OK_FAILED(__e) ((__e) ? ((void)0) : (failed(__FILE__, __LINE__,  #__e)))

// macros for testing certain conditions
#define ASSERT_TRUE(__e) __OK_FAILED(__e)
#define ASSERT_EQUALS(__e, __v) __OK_FAILED((__e) == (__v))
#define ASSERT_GREATER(__e, __v) __OK_FAILED((__e) > (__v))
#define ASSERT_LESS(__e, __v) __OK_FAILED((__e) < (__v))


#ifdef __cplusplus
}
#endif

#endif
