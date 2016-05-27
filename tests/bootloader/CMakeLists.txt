# Bootloader test

add_executable(test_bootloader test_bootloader.c)
target_link_libraries(test_bootloader ubirch::${BOARD}::firmware)
prepare_flash(TARGET test_bootloader)
