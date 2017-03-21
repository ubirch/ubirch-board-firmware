add_executable(power_switch power_switch.c)
target_link_libraries(power_switch board)
target_link_libraries(power_switch firmware)
prepare_flash(TARGET power_switch)
#prepare_flash(TARGET power_switch JLINK DEVICE MK82FN256xxx15)


