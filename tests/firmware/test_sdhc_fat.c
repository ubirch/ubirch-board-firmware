#include <board.h>

// we only need this if there is an SDHC port on the chip
#if defined(BOARD_SDHC)

#include <ubirch/sdhc.h>
#include <ff.h>
#include <diskio.h>
#include <fsl_mpu.h>
#include "test.h"

static FATFS fatfs;

int test_sdhc_fat(void) {
  sdhc_init();
  MPU_Enable(MPU, false);

  uint32_t card_inserted = GPIO_ReadPinInput(BOARD_SDHC_DET_GPIO, BOARD_SDHC_DET_PIN);
  ASSERT_TRUE(card_inserted);
  //  delay(1000U);

  const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
  ASSERT_TRUE(f_mount(&fatfs, driverNumberBuffer, 0U) == FR_OK);
  ASSERT_TRUE(f_chdrive((char const *) &driverNumberBuffer[0U]) == FR_OK);

  FIL testFileObject;
  ASSERT_TRUE(!f_open(&testFileObject, _T("/test.txt"), FA_READ));

  uint8_t buffer[128];
  UINT size;
  memset(buffer, 0U, sizeof(buffer));
  ASSERT_TRUE(!f_read(&testFileObject, buffer, sizeof(buffer), &size));
  ASSERT_TRUE(size != 0);
  ASSERT_LESS(size, 128);

  ASSERT_TRUE(!f_close(&testFileObject));

  return 0;
}
#else
int test_sdhc_fat(void) {
  return 0;
}
#endif



