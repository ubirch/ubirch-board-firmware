add_executable(modem_power modem_power.c modem_power.c)
target_link_libraries(modem_power board)
target_link_libraries(modem_power firmware)
prepare_flash(TARGET modem_power)
#prepare_flash(TARGET modem_power JLINK DEVICE MK82FN256xxx15)


