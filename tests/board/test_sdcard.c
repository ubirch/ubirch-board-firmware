#include <ff.h>
#include <fsl_sdhc.h>
#include <fsl_debug_console.h>
#include <fsl_port.h>
#include <diskio.h>
#include <fsl_gpio.h>
#include <ubirch/timer.h>
#include <fsl_card.h>
#include "sdhc_config.h"

FATFS g_fileSystem;
FIL g_fileObject;

#define BUFFER_SIZE 255
char g_bufferWrite[BUFFER_SIZE];
char g_bufferRead[BUFFER_SIZE];

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

static uint32_t g_sdhcAdmaTable[SDHC_ADMA_TABLE_WORDS];

status_t sdhc_transfer_function(SDHC_Type *base, sdhc_transfer_t *content) {
  status_t error = kStatus_Success;
  error = SDHC_TransferBlocking(base, g_sdhcAdmaTable, SDHC_ADMA_TABLE_WORDS, content);
  return error;
}

void status_messages(status_t s) {
  switch (s) {
    case kStatus_SDMMC_NotSupportYet: { PRINTF("Haven't supported\r\n"); break; }
    case kStatus_SDMMC_TransferFailed: { PRINTF("Send command failed\r\n"); break; }
    case kStatus_SDMMC_SetCardBlockSizeFailed: { PRINTF("Set block size failed\r\n"); break; }
    case kStatus_SDMMC_HostNotSupport: { PRINTF("Host doesn't support\r\n"); break; }
    case kStatus_SDMMC_CardNotSupport: { PRINTF("Card doesn't support\r\n"); break; }
    case kStatus_SDMMC_AllSendCidFailed: { PRINTF("Send CID failed\r\n"); break; }
    case kStatus_SDMMC_SendRelativeAddressFailed: { PRINTF("Send relative address failed\r\n"); break; }
    case kStatus_SDMMC_SendCsdFailed: { PRINTF("Send CSD failed\r\n"); break; }
    case kStatus_SDMMC_SelectCardFailed: { PRINTF("Select card failed\r\n"); break; }
    case kStatus_SDMMC_SendScrFailed: { PRINTF("Send SCR failed\r\n"); break; }
    case kStatus_SDMMC_SetDataBusWidthFailed: { PRINTF("Set bus width failed\r\n"); break; }
    case kStatus_SDMMC_GoIdleFailed: { PRINTF("Go idle failed\r\n"); break; }
    case kStatus_SDMMC_HandShakeOperationConditionFailed: { PRINTF("Send Operation Condition failed\r\n"); break; }
    case kStatus_SDMMC_SendApplicationCommandFailed: { PRINTF("Send application command failed\r\n"); break; }
    case kStatus_SDMMC_SwitchFailed: { PRINTF("Switch command failed\r\n"); break; }
    case kStatus_SDMMC_StopTransmissionFailed: { PRINTF("Stop transmission failed\r\n"); break; }
    case kStatus_SDMMC_WaitWriteCompleteFailed: { PRINTF("Wait write complete failed\r\n"); break; }
    case kStatus_SDMMC_SetBlockCountFailed: { PRINTF("Set block count failed\r\n"); break; }
    case kStatus_SDMMC_SetRelativeAddressFailed: { PRINTF("Set relative address failed\r\n"); break; }
    case kStatus_SDMMC_SwitchHighSpeedFailed: { PRINTF("Switch high speed failed\r\n"); break; }
    case kStatus_SDMMC_SendExtendedCsdFailed: { PRINTF("Send EXT_CSD failed\r\n"); break; }
    case kStatus_SDMMC_ConfigureBootFailed: { PRINTF("Configure boot failed\r\n"); break; }
    case kStatus_SDMMC_ConfigureExtendedCsdFailed: { PRINTF("Configure EXT_CSD failed\r\n"); break; }
    case kStatus_SDMMC_EnableHighCapacityEraseFailed: { PRINTF("Enable high capacity erase failed\r\n"); break; }
    case kStatus_SDMMC_SendTestPatternFailed: { PRINTF("Send test pattern failed\r\n"); break; }
    case kStatus_SDMMC_ReceiveTestPatternFailed: { PRINTF("Receive test pattern failed\r\n"); break; }
    case kStatus_Success: { PRINTF("OK"); break; }
    default: { PRINTF("??? 0x%x\r\n", s); break; }
  }

}

void test_sdhc(void) {
  init_sdhc_pins();

  sd_card_t sd;
  sdhc_config_t *sdhcConfig = &(sd.host.config);

  /* Initializes the SDHC. */
  sdhcConfig->cardDetectDat3 = false;
  sdhcConfig->endianMode = kSDHC_EndianModeLittle;
  sdhcConfig->dmaMode = kSDHC_DmaModeAdma2;
  sdhcConfig->readWatermarkLevel = 0x80U;
  sdhcConfig->writeWatermarkLevel = 0x80U;
  SDHC_Init(SDHC, sdhcConfig);

  sd.host.base = SDHC;
  sd.host.sourceClock_Hz = CLOCK_GetFreq(kCLOCK_CoreSysClk);
  sd.host.transfer = sdhc_transfer_function;


  status_messages(SD_Init(&sd));

  uint8_t buffer[512];
  memset(buffer, 0b10101010, sizeof(buffer));

  PRINTF("\r\nRead/Write/Erase the card\r\n");
  status_messages(SD_WriteBlocks(&sd, buffer, 0, 1));
  memset(buffer, 0, sizeof(buffer));
  status_messages(SD_ReadBlocks(&sd, buffer, 0, 1));
//  for(int i = 0; i < 512; i++) {
//    if(buffer[i] != 0b10101010) PRINTF("%d: data error [%08b]", i, buffer[i]);
//  }
  status_messages(SD_EraseBlocks(&sd, 0, 1));
  status_messages(SD_ReadBlocks(&sd, buffer, 0, 1));
//  memset(buffer, 0xff, sizeof(buffer));
//  for(int i = 0; i < 512; i++) {
//    if(buffer[i] != 0) PRINTF("%d: data error [%08b]", i, buffer[i]);
//  }
  SD_Deinit(&sd);
}

void test_sdcard(void) {
  init_sdhc_pins();

  FRESULT error;
  DIR directory; /* Directory object */
  FILINFO fileInformation;
  UINT bytesWritten;
  UINT bytesRead;
  const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
  bool failedFlag = false;
  char ch = '0';


  PRINTF("\r\nFATFS example to demonstrate how to use FATFS with SD card.\r\n");

  PRINTF("\r\nPlease insert a card into board.\r\n");
/* Wait the card to be inserted. */
  while (!GPIO_ReadPinInput(GPIOE, 7U)) {
  }
  PRINTF("Detected SD card inserted.\r\n");
  /* Delat some time to make card stable. */
  delay(1000U);

  if (f_mount(&g_fileSystem, driverNumberBuffer, 0U)) {
    PRINTF("Mount volume failed.\r\n");
    return;
  }

#if (_FS_RPATH >= 2U)
  error = f_chdrive((char const *) &driverNumberBuffer[0U]);
  if (error) {
    PRINTF("Change drive failed.\r\n");
    return;
  }
#endif

#if _USE_MKFS
    PRINTF("\r\nMake file system......The time may be long if the card capacity is big.\r\n");
    if (f_mkfs(driverNumberBuffer, 1U, 0U))
    {
      PRINTF("Make file system failed.\r\n");
      return;
    }
#endif /* _USE_MKFS */

  PRINTF("\r\nCreate directory......\r\n");
  error = f_mkdir(_T("/dir_1"));
  if (error) {
    if (error == FR_EXIST) {
      PRINTF("Directory exists.\r\n");
    }
    else {
      PRINTF("Make directory failed.\r\n");
      return;
    }
  }

  PRINTF("\r\nCreate a file in that directory......\r\n");
  error = f_open(&g_fileObject, _T("/dir_1/f_1.dat"), (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
  if (error) {
    if (error == FR_EXIST) {
      PRINTF("File exists.\r\n");
    }
    else {
      PRINTF("Open file failed.\r\n");
      return;
    }
  }

  PRINTF("\r\nCreate a directory in that directory......\r\n");
  error = f_mkdir(_T("/dir_1/dir_2"));
  if (error) {
    if (error == FR_EXIST) {
      PRINTF("Directory exists.\r\n");
    }
    else {
      PRINTF("Directory creation failed.\r\n");
      return;
    }
  }

  PRINTF("\r\nList the file in that directory......\r\n");
  if (f_opendir(&directory, "/dir_1")) {
    PRINTF("Open directory failed.\r\n");
    return;
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
    }
    else {
      PRINTF("General file : %s.\r\n", fileInformation.fname);
    }
  }

  memset(g_bufferWrite, 'a', sizeof(g_bufferWrite));
  g_bufferWrite[BUFFER_SIZE - 2U] = '\r';
  g_bufferWrite[BUFFER_SIZE - 1U] = '\n';

  PRINTF("\r\nWrite/read file until encounters error......\r\n");
  while (true) {
    if (failedFlag || (ch == 'q')) {
      break;
    }

    PRINTF("\r\nWrite to above created file.\r\n");
    error = f_write(&g_fileObject, g_bufferWrite, sizeof(g_bufferWrite), &bytesWritten);
    if ((error) || (bytesWritten != sizeof(g_bufferWrite))) {
      PRINTF("Write file failed. \r\n");
      failedFlag = true;
      continue;
    }

    /* Move the file pointer */
    if (f_lseek(&g_fileObject, 0U)) {
      PRINTF("Set file pointer position failed. \r\n");
      failedFlag = true;
      continue;
    }

    PRINTF("Read from above created file.\r\n");
    memset(g_bufferRead, 0U, sizeof(g_bufferRead));
    error = f_read(&g_fileObject, g_bufferRead, sizeof(g_bufferRead), &bytesRead);
    if ((error) || (bytesRead != sizeof(g_bufferRead))) {
      PRINTF("Read file failed. \r\n");
      failedFlag = true;
      continue;
    }

    PRINTF("Compare the read/write content......\r\n");
    if (memcmp(g_bufferWrite, g_bufferRead, sizeof(g_bufferWrite))) {
      PRINTF("Compare read/write content isn't consistent.\r\n");
      failedFlag = true;
      continue;
    }
    PRINTF("The read/write content is consistent.\r\n");

    PRINTF("\r\nInput 'q' to quit read/write.\r\nInput other char to read/write file again.\r\n");
    ch = GETCHAR();
    PUTCHAR(ch);
  }
  PRINTF("\r\nThe example will not read/write file again.\r\n");

  if (f_close(&g_fileObject)) {
    PRINTF("\r\nClose file failed.\r\n");
    return;
  }

  while (true) {
  }

}