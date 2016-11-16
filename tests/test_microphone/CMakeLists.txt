add_executable(test_mic test_mic.c)
target_link_libraries(test_mic board)
target_link_libraries(test_mic firmware)
prepare_flash(TARGET test_mic)
