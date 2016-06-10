# Firmware Tests

add_executable(test_firmware
  test_firmware.c
  test_i2c.c
  i2c/test_isl29125.c
  test_timer.c
  test_rtc.c
  )
target_link_libraries(test_firmware ubirch::${BOARD}::firmware)
#prepare_flash(TARGET test_firmware)
prepare_flash(TARGET test_firmware JLINK DEVICE MKL82Z128xxx7)
#prepare_flash(TARGET test_firmware JLINK DEVICE MK82FN256xxx15)
