# Board and Firmware Tests

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/board/${BOARD}")
  # add alias to access the package target libraries like in downstream projects
  add_library(ubirch::${BOARD}::board ALIAS board)
  add_library(ubirch::${BOARD}::firmware ALIAS firmware)

  add_subdirectory(board/${BOARD})
  add_subdirectory(firmware)
endif()
