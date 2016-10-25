#include <ff.h>
#include <fsl_sdhc.h>
#include <fsl_debug_console.h>
#include <fsl_port.h>
#include <fsl_gpio.h>
#include <ubirch/timer.h>
#include <fsl_mpu.h>
#include <diskio.h>

#define BUFFER_SIZE 255
static FATFS g_fileSystem; /* File system object */
static FIL g_fileObject;   /* File object */

//static uint8_t g_bufferWrite[BUFFER_SIZE]; /* Write buffer */
//static uint8_t g_bufferRead[BUFFER_SIZE];  /* Read buffer */

void init_sdhc_pins() {
  port_pin_config_t config = {0};
  config.pullSelect = kPORT_PullUp;
  config.driveStrength = kPORT_HighDriveStrength;
  config.mux = kPORT_MuxAlt4;

  CLOCK_EnableClock(kCLOCK_PortE);

  // SD pins
  PORT_SetPinConfig(PORTE, 0U, &config);
  PORT_SetPinConfig(PORTE, 1U, &config);
  PORT_SetPinConfig(PORTE, 2U, &config);
  PORT_SetPinConfig(PORTE, 3U, &config);
  PORT_SetPinConfig(PORTE, 4U, &config);
  PORT_SetPinConfig(PORTE, 5U, &config);

  // SD CD pin
  config.driveStrength = kPORT_LowDriveStrength;
  config.mux = kPORT_MuxAsGpio;
  PORT_SetPinConfig(PORTE, 7U, &config);
  const gpio_pin_config_t IN = {kGPIO_DigitalInput, false};
  GPIO_PinInit(GPIOE, 7U, &IN);
}

int pcm__sdhc(uint8_t *g_bufferWrite) {
//  init_sdhc_pins();

  FRESULT error;
  DIR directory; /* Directory object */
  FILINFO fileInformation;
  UINT bytesWritten;
//  UINT bytesRead;
  const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};

  MPU_Enable(MPU, false);

  PRINTF("\r\nFATFS example to demonstrate how to use FATFS with SD card.\r\n");

  PRINTF("\r\nPlease insert a card into board.\r\n");

  /* Wait the card to be inserted. */
  while (!(GPIO_ReadPinInput(GPIOE, 7U))) {
  }
  PRINTF("Detected SD card inserted.\r\n");
  /* Delat some time to make card stable. */
  delay(1000U);

  if (f_mount(&g_fileSystem, driverNumberBuffer, 0U)) {
    PRINTF("Mount volume failed.\r\n");
    return -1;
  }

#if (_FS_RPATH >= 2U)
  error = f_chdrive(&driverNumberBuffer[0U]);
  if (error) {
    PRINTF("Change drive failed.\r\n");
    return -1;
  }
#endif

#if _USE_MKFS
  PRINTF("\r\nMake file system......The time may be long if the card capacity is big.\r\n");
if (f_mkfs(driverNumberBuffer, 1U, 0U))
{
    PRINTF("Make file system failed.\r\n");
    return -1;
}
#endif /* _USE_MKFS */

  FIL testFileObject;
//  PRINTF("\r\nRead from static test file\r\n");
//  error = f_open(&testFileObject, _T("/new_music.pcm"), FA_CREATE_NEW);
//  if (error) {
//    PRINTF("Open file failed (%d).\r\n", error);
//    return -1;
//  }

//  memset(g_bufferRead, 0U, sizeof(g_bufferRead));
//  error = f_read(&testFileObject, g_bufferRead, sizeof(g_bufferRead), &bytesRead);
//  if (error) {
//    PRINTF("Read file failed. \r\n");
//  }
//  PRINTF("bytes read: %d\r\n", bytesRead);
//  PRINTF("----\r\n");
//  PRINTF("%s", g_bufferRead);
//  PRINTF("----\r\n");
//
//
//  if (f_close(&testFileObject)) {
//    PRINTF("\r\nClose file failed.\r\n");
//    return -1;
//  }
//
  PRINTF("\r\nCreate directory......\r\n");
  error = f_mkdir(_T("/music"));
  if (error) {
    if (error == FR_EXIST) {
      PRINTF("Directory exists.\r\n");
    } else {
      PRINTF("Make directory failed.\r\n");
      return -1;
    }
  }

  PRINTF("\r\nCreate a file in that directory......\r\n");
  error = f_open(&g_fileObject, _T("/dir_1/dj.pcm"), (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
  if (error) {
    if (error == FR_EXIST) {
      PRINTF("File exists.\r\n");
    } else {
      PRINTF("Open file failed.\r\n");
      return -1;
    }
  }

//  PRINTF("\r\nCreate a directory in that directory......\r\n");
//  error = f_mkdir(_T("/dir_1/dir_2"));
//  if (error) {
//    if (error == FR_EXIST) {
//      PRINTF("Directory exists.\r\n");
//    } else {
//      PRINTF("Directory creation failed.\r\n");
//      return -1;
//    }
//  }

  PRINTF("\r\nList the file in that directory......\r\n");
  if (f_opendir(&directory, "/dir_1")) {
    PRINTF("Open directory failed.\r\n");
    return -1;
  }

  for (;;) {
    error = f_readdir(&directory, &fileInformation);

    /* To the end. */
    if ((error != FR_OK) || (fileInformation.fname[0U] == 0U)) {
      break;
    }
    if (fileInformation.fname[0] == '.') {
      continue;
    }
    if (fileInformation.fattrib & AM_DIR) {
      PRINTF("Directory file : %s.\r\n", fileInformation.fname);
    } else {
      PRINTF("General file : %s.\r\n", fileInformation.fname);
    }
  }

//  memset(g_bufferWrite, 'a', sizeof(g_bufferWrite));
  g_bufferWrite[BUFFER_SIZE - 2U] = '\r';
  g_bufferWrite[BUFFER_SIZE - 1U] = '\n';

  PRINTF("\r\nWrite/read file ...\r\n");

  PRINTF("\r\nWrite to above created file.\r\n");
  error = f_write(&g_fileObject, g_bufferWrite, sizeof(g_bufferWrite), &bytesWritten);
  if ((error) || (bytesWritten != sizeof(g_bufferWrite))) {
    PRINTF("Write file failed. \r\n");
    return -1;
  }

//  /* Move the file pointer */
//  if (f_lseek(&g_fileObject, 0U)) {
//    PRINTF("Set file pointer position failed. \r\n");
//    return -1;
//  }
//
//  PRINTF("Read from above created file.\r\n");
//  memset(g_bufferRead, 0U, sizeof(g_bufferRead));
//  error = f_read(&g_fileObject, g_bufferRead, sizeof(g_bufferRead), &bytesRead);
//  if ((error) || (bytesRead != sizeof(g_bufferRead))) {
//    PRINTF("Read file failed. \r\n");
//    return -1;
//  }
//
//  PRINTF("Compare the read/write content......\r\n");
//  if (memcmp(g_bufferWrite, g_bufferRead, sizeof(g_bufferWrite))) {
//    PRINTF("Compare read/write content isn't consistent.\r\n");
//    return -1;
//  }
//  PRINTF("The read/write content is consistent.\r\n");
//
//
//  PRINTF("\r\nThe example will not read/write file again.\r\n");

  if (f_close(&g_fileObject)) {
    PRINTF("\r\nClose file failed.\r\n");
    return -1;
  }

  return 0;
}
