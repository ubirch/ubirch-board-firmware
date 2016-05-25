# I2C test programs

# I2C scanning test
add_executable(test_i2c
  test_i2c.c
  test_isl29125.c
  )
target_link_libraries(test_i2c ubirch::${BOARD}::firmware)
prepare_flash(TARGET test_i2c)
