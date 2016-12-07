# Firmware Tests

add_executable(test_firmware
  test.c
  test_firmware.c
  test_i2c.c
  i2c/test_isl29125.c
  i2c/test_bmp180.c
  i2c/test_bme280.c
  i2c/test_ssd1306.c
  test_timer.c
  test_rtc.c
  test_ws2812b_flexio.c
  test_sdhc_fat.c
  )
target_include_directories(test_firmware PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}})
target_link_libraries(test_firmware ubirch::${BOARD}::firmware)
#prepare_flash(TARGET test_firmware)
#prepare_flash(TARGET test_firmware JLINK DEVICE MKL82Z128xxx7)
prepare_flash(TARGET test_firmware JLINK DEVICE MK82FN256xxx15)
