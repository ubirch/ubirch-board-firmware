# Board and Firmware Tests

add_library(ubirch::${BOARD}::board ALIAS board)
add_library(ubirch::${BOARD}::firmware ALIAS firmware)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/board/${BOARD}")
  # add alias to access the package target libraries like in downstream projects
  add_subdirectory(board/${BOARD})
endif()

add_subdirectory(cc430)
add_subdirectory(firmware)
add_subdirectory(identify)
add_subdirectory(modem)
add_subdirectory(modem_http)
add_subdirectory(modem_power)
add_subdirectory(power_mode)
add_subdirectory(power_switch)
add_subdirectory(power_mode_extpin)
add_subdirectory(power_mode_rtc)