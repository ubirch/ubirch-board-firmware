add_executable(power_mode power_mode.c clock_mcg.c)
target_link_libraries(power_mode board)
target_link_libraries(power_mode firmware)
prepare_flash(TARGET power_mode)
#prepare_flash(TARGET power_mode JLINK DEVICE MK82FN256xxx15)


