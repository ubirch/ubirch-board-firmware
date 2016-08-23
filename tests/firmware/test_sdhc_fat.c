#include <ubirch/timer.h>
#include <ubirch/sdhc.h>
#include <ff.h>
#include <diskio.h>
#include <fsl_mpu.h>
#include "test.h"

static FATFS fatfs;

int test_sdhc_fat(void) {
#if defined(FSL_FEATURE_SOC_SDHC_COUNT) && FSL_FEATURE_SOC_SDHC_COUNT > 0
  sdhc_init();
  MPU_Enable(MPU, false);

  FRESULT error;
  uint32_t card_inserted = GPIO_ReadPinInput(BOARD_SDHC_DET_GPIO, BOARD_SDHC_DET_PIN);
  ASSERT_TRUE(card_inserted);

  const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
  ASSERT_TRUE(f_mount(&fatfs, driverNumberBuffer, 0U) == FR_OK);
  ASSERT_TRUE(f_chdrive((char const *) &driverNumberBuffer[0U]) == FR_OK);
  delay(1000U);

  FIL testFileObject;
  PRINTF("\r\nRead from static test file\r\n");
  error = f_open(&testFileObject, _T("/test.txt"), FA_READ);
  if (error) {
    PRINTF("Open file failed.\r\n");
    return -1;
  }

  PRINTF("O");
  uint8_t buffer[128];
  UINT size;
  memset(buffer, 0U, sizeof(buffer));
  error = f_read(&testFileObject, buffer, sizeof(buffer), &size);
  PRINTF("OK");
  ASSERT_TRUE(!error);
  ASSERT_TRUE(size != 0);
  ASSERT_LESS(size, 128);

  error = f_close(&testFileObject);
  ASSERT_TRUE(!error);

#else
  PRINTF("NOT AVAILABLE\r\n")
#endif

  return 0;
}



