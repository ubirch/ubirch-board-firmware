# Firmware code

if (BOARD MATCHES "ubirch1r03")
  set(CELL_SRCS
    ubirch/m66/m66_core.c
    ubirch/m66/m66_http.c
    ubirch/m66/m66_ops.c
    ubirch/m66/m66_parser.c
    )
  set(SDHC_SRCS
    ubirch/sdhc.c
    ${FATFS_MIDDLEWARE_SRCS}
    )
else ()
  set(FIRMWARE_SRCS ${FIRMWARE_SRCS}
    ubirch/sim800h/sim800h_core.c
    ubirch/sim800h/sim800h_http.c
    ubirch/sim800h/sim800h_ops.c
    ubirch/sim800h/sim800h_parser.c
    )
endif ()

add_library(firmware
  ubirch/dbgutil.c
  ubirch/i2c.c
  ubirch/i2c/isl29125.c
  ubirch/i2c/ssd1306.c
  ubirch/i2c/bosch_sensortec/bme280.c
  ubirch/i2c/bosch_sensortec/bmp180.c
  ubirch/i2c/bmp180.c
  ubirch/i2c/bme280.c
#  ubirch/i2c/tmp112.c
  ubirch/rtc.c
  ubirch/timer.c
  ubirch/ws2812b.c
  ${CELL_SRCS}
  ${SDHC_SRCS}
  ubirch/sub-GHz/rf_lpuart.h ubirch/sub-GHz/rf_lpuart.c)
target_include_directories(firmware PUBLIC
  ${FATFS_MIDDLEWARE_INC}
  ${CMAKE_CURRENT_SOURCE_DIR}/ubirch/sdhc
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
target_link_libraries(firmware board KinetisSDK::${MCU}::ksdk20)
if (BOARD MATCHES "ubirch1r03")
  target_link_libraries(firmware KinetisSDK::${MCU}::sdmmc)
endif ()

# only do the full debug output in "Debug"
if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(firmware PUBLIC -DNCIODEBUG)
endif ()
