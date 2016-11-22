add_executable(identify identify.c)
target_link_libraries(identify board)
target_link_libraries(identify firmware)
prepare_flash(TARGET identify)


