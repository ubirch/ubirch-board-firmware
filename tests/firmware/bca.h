// types and definitions for the bootloader configuration area

/*!
 * @brief Defines the structure to set the Bootloader Configuration Area
 *
 * This type of variable is used to set the Bootloader Configuration Area
 * of the chip.
 */
typedef struct SystemBootloaderConfig {
    uint32_t tag;               /*!< Magic number to verify bootloader configuration is valid. Must be set to 'kcfg'. */
    uint32_t crcStartAddress;   /*!< Start address for application image CRC check. If the bits are all set then Kinetis
                                   bootloader by default will not perform any CRC check. */
    uint32_t crcByteCount;      /*!< Byte count for application image CRC check. If the bits are all set then Kinetis
                                   bootloader by default will not prform any CRC check. */
    uint32_t crcExpectedValue;  /*!< Expected CRC value for application CRC check. If the bits are all set then Kinetis
                                   bootloader by default will not perform any CRC check.*/
    uint8_t enabledPeripherals; /*!< Bitfield of peripherals to enable.
                                     bit 0 - LPUART, bit 1 - I2C, bit 2 - SPI, bit 4 - USB
                                     Kinetis bootloader will enable the peripheral if corresponding bit is set to 1. */
    uint8_t i2cSlaveAddress;    /*!< If not 0xFF, used as the 7-bit I2C slave address. If 0xFF, defaults to 0x10 for I2C
                                   slave address */
    uint16_t peripheralDetectionTimeoutMs; /*!< Timeout in milliseconds for active peripheral detection. If 0xFFFF,
                                              defaults to 5 seconds. */
    uint16_t usbVid; /*!< Sets the USB Vendor ID reported by the device during enumeration. If 0xFFFF, it defaults to
                        0x15A2. */
    uint16_t usbPid; /*!< Sets the USB Product ID reported by the device during enumeration. */
    uint32_t usbStringsPointer; /*!< Sets the USB Strings reported by the device during enumeration. */
    uint8_t clockFlags;   /*!< The flags in the clockFlags configuration field are enabled if the corresponding bit is
                             cleared (0).
                              bit 0 - HighSpeed Enable high speed mode (i.e., 48 MHz).	*/
    uint8_t clockDivider; /*!< Inverted value of the divider to use for core and bus clocks when in high speed mode */
    uint8_t bootFlags; /*!< If bit 0 is cleared, then Kinetis bootloader will jump to either Quad SPI Flash or internal
                          flash image depending on FOPT BOOTSRC_SEL bits.
                           If the bit is set, then Kinetis bootloader will prepare for host communication over serial
                          peripherals. */
    uint8_t RESERVED1;
    uint32_t RESERVED2;
    uint32_t keyBlobPointer; /*!< A pointer to the keyblob data in memory. */
    uint8_t RESERVED3[8];
    uint32_t qspiConfigBlockPtr; /*!< A pointer to the QSPI config block in internal flash array. */
    uint8_t RESERVED4[12];
} system_bootloader_config_t;

/* Bootlader configuration area */
#if defined(__IAR_SYSTEMS_ICC__)
/* Pragma to place the Bootloader Configuration Array on correct location defined in linker file. */
#pragma language = extended
#pragma location = "BootloaderConfig"
__root const system_bootloader_config_t BootloaderConfig @"BootloaderConfig" =
#elif defined(__GNUC__)
__attribute__((section(".BootloaderConfig"))) const system_bootloader_config_t BootloaderConfig =
#elif defined(__CC_ARM)
__attribute__((section("BootloaderConfig"))) const system_bootloader_config_t BootloaderConfig __attribute__((used)) =
#else
#error Unsupported compiler!
#endif
{
.tag = 0x6766636BU,                      /* Magic Number */
.crcStartAddress = 0xFFFFFFFFU,          /* Disable CRC check */
.crcByteCount = 0xFFFFFFFFU,             /* Disable CRC check */
.crcExpectedValue = 0xFFFFFFFFU,         /* Disable CRC check */
.enabledPeripherals = 0x10,              /* Enable all peripherals */
.i2cSlaveAddress = 0xFF,                 /* Use default I2C address */
.peripheralDetectionTimeoutMs = 0xFFFFU, /* Use default timeout */
.usbVid = 0xFFFFU,                       /* Use default USB Vendor ID */
.usbPid = 0xFFFFU,                       /* Use default USB Product ID */
.usbStringsPointer = 0xFFFFFFFFU,        /* Use default USB Strings */
.clockFlags = 0x01,                      /* Enable High speed mode */
.clockDivider = 0xFF,                    /* Use clock divider 1 */
.bootFlags = 0x01,                       /* Enable communication with host */
.keyBlobPointer = 0xFFFFFFFFU,           /* No keyblob data */
.qspiConfigBlockPtr = 0xFFFFFFFFU        /* No QSPI configuration */
};
