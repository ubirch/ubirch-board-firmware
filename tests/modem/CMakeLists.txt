add_executable(modem modem.c modem.c)
target_link_libraries(modem board)
target_link_libraries(modem firmware)
prepare_flash(TARGET modem)


