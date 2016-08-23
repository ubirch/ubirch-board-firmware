# Board and Firmware Tests

# add alias to access the package target libraries like in downstream projects
add_library(ubirch::${BOARD}::firmware ALIAS firmware)
add_library(ubirch::${BOARD}::board ALIAS board)

add_subdirectory(firmware)
add_subdirectory(board)
