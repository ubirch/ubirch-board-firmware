# Test ubirch#1 r0.3 k0.1 board

if (BOARD MATCHES "ubirch1r03k01")
  add_executable(test_board
    test_board_ubirch1r03k01.c
    )
  target_include_directories(test_board PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}})
  target_link_libraries(test_board ubirch::${BOARD}::board)
  #prepare_flash(TARGET test_board)
  #prepare_flash(TARGET test_board JLINK DEVICE MKL82Z128xxx7)
  prepare_flash(TARGET test_board JLINK DEVICE MK82FN256xxx15)
endif ()
