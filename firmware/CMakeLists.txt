# Firmware code

add_library(firmware
  ubirch/sim800h/sim800h_core.c
  ubirch/sim800h/sim800h_http.c
  ubirch/sim800h/sim800h_ops.c
  ubirch/sim800h/sim800h_parser.c
  ubirch/crypto.c
  ubirch/dbgutil.c
  ubirch/i2c.c
  ubirch/isl29125.c
  ubirch/rtc.c
  ubirch/ssd1306.c
  ubirch/timer.c
  )
target_include_directories(firmware PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(firmware board wolfcrypt)
