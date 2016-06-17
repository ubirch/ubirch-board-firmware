# Firmware code

add_library(firmware
  ubirch/sim800h/sim800h_core.c
  ubirch/sim800h/sim800h_http.c
  ubirch/sim800h/sim800h_ops.c
  ubirch/sim800h/sim800h_parser.c
  ubirch/dbgutil.c
  ubirch/i2c.c
  ubirch/i2c/isl29125.c
  ubirch/i2c/ssd1306.c
  ubirch/rtc.c
  ubirch/timer.c
  ubirch/i2c/bmp180.c
  )
target_include_directories(firmware PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(firmware board KinetisSDK::${MCU}::ksdk20)
# only do the full debug output in "Debug"
if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(firmware PUBLIC -DNCIODEBUG)
endif ()
