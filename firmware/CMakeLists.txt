# Firmware code

if (BOARD MATCHES "ubirch1r03")
  set(CELLCHIP_SRCS
    ubirch/m66/m66_core.c
    ubirch/m66/m66_http.c
    ubirch/m66/m66_ops.c
    ubirch/m66/m66_parser.c
    )
else ()
  set(CELLCHIP_SRCS
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
  ubirch/rtc.c
  ubirch/timer.c
  ${CELLCHIP_SRCS}
  )
target_include_directories(firmware PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(firmware board KinetisSDK::${MCU}::ksdk20)
# only do the full debug output in "Debug"
if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(firmware PUBLIC -DNCIODEBUG)
endif ()
