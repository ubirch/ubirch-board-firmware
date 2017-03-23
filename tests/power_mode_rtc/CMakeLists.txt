add_executable(rtc_power_mode power_mode_rtc.c clock_mcg.c)
target_link_libraries(rtc_power_mode board)
target_link_libraries(rtc_power_mode firmware)
prepare_flash(TARGET rtc_power_mode)
#prepare_flash(TARGET power_mode JLINK DEVICE MK82FN256xxx15)


