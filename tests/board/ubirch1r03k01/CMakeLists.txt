# Test different aspects of the firmware
# these are tests for the ubirch#1 r03-k

if (BOARD MATCHES "ubirch1r03k01")
  set(SRCS
    test_board_ubirch1r03k01.c
    test_gpio.c
    test_rgb_flexio.c
    test_touch.c
    support.c
    test_rgb_dma.c
    test_audio.c
    test_pir.c
    test_quectel.c
    test_sdcard.c
    test_i2s.c
    test_rf_lpuart.c
    ${FATFS_MIDDLEWARE_SRC}
    )

  add_executable(test_board ${SRCS})
  target_compile_definitions(test_board PRIVATE -DARM_MATH_CM4)

  target_include_directories(test_board PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_include_directories(test_board PRIVATE ${FATFS_MIDDLEWARE_INC})
  target_link_libraries(test_board ubirch::${BOARD}::board)
  target_link_libraries(test_board ubirch::${BOARD}::firmware)
  target_link_libraries(test_board KinetisSDK::${MCU}::sdmmc)
  prepare_flash(TARGET test_board)
  #  prepare_flash(TARGET test_board JLINK DEVICE MKL82Z128xxx7)
  #  prepare_flash(TARGET test_board JLINK DEVICE MK82FN256xxx15)

  # copy the config.h.template to config.h in case it is not there; it is ignored by .git!
  if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.h")
    message(AUTHOR_WARNING "In directory '${CMAKE_CURRENT_SOURCE_DIR}': installing the template config.h, please edit!")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.template ${CMAKE_CURRENT_SOURCE_DIR}/config.h COPYONLY)
  endif ()

endif ()
