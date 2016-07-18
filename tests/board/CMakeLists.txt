# Test ubirch#1 r0.3 k0.1 board

if (BOARD MATCHES "ubirch1r03k01")
  add_executable(test_board
    test_board_ubirch1r03k01.c
    test_audio.c
    test_gpio.c
    test_pir.c
    test_rgb.c
    test_quectel.c
    test_touch.c
    test_sdcard.c
    support.c
    ${FATFS_MIDDLEWARE_SRCS}
    ffconf.c
    )
  target_include_directories(test_board PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_include_directories(test_board PUBLIC ${FATFS_MIDDLEWARE_INC})
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
