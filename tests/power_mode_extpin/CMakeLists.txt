add_executable(extpin_power_mode power_mode_extpin.c clock_mcg.c)
target_link_libraries(extpin_power_mode board)
target_link_libraries(extpin_power_mode firmware)
prepare_flash(TARGET extpin_power_mode)
#prepare_flash(TARGET power_mode JLINK DEVICE MK82FN256xxx15)


