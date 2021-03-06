cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_TOOLCHAIN_FILE ../ubirch-arm-toolchain/cmake/ubirch-arm-gcc-toolchain.cmake CACHE STRING "toolchain")

# set board type (ubirch1r02, FRDM-K82F, FRDM-KL82Z)
# you don't need to set it here, use 'cmake -DBOARD=<board> ..' during setup
# the board setting will determine the used MCU
set(BOARD "ubirch1r02" CACHE STRING "board name")
message(STATUS "selected board: ${BOARD}")

# board specific code (pins, peripherals etc.)
if (BOARD MATCHES "ubirch1|ubridge")
  set(MCU "MK82F25615")
  set(BOARD_DIR ${BOARD})
elseif(BOARD MATCHES "usense")
  set(MCU "MKL82Z7")
  set(BOARD_DIR ${BOARD})
elseif (BOARD MATCHES "FRDM-K82F")
  set(MCU "MK82F25615")
  set(BOARD_DIR frdm_k82f)
elseif (BOARD MATCHES "FRDM-KL82Z")
  set(MCU "MKL82Z7")
  set(BOARD_DIR frdm_kl82z)
else ()
  message(FATAL_ERROR "Please set a board type: -DBOARD=<...> (i.e. ubirch1r02, FRDM-K82F, FRDM-KL82Z)")
endif ()

project(ubirch-firmware C CXX ASM)

require(PACKAGE KinetisSDK MCU ${MCU} VERSION 2.0 BUILD_TYPE ${CMAKE_BUILD_TYPE})

# board and firmware code
add_subdirectory(board/${BOARD_DIR})
add_subdirectory(firmware)

provide(PACKAGE ubirch VERSION 1.0 BOARD ${BOARD} TARGETS board firmware)

# add tests
add_subdirectory(tests)


# Generate Documentation for firmware and board
find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
  file(GLOB IMAGES ubirch-board-firmware/board/*/*.jpg)
  list(LENGTH IMAGES n)
  if (n GREATER 0)
    add_custom_command(TARGET doc
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${IMAGES} ${CMAKE_CURRENT_BINARY_DIR}/docs/html
      )
  endif ()
endif (DOXYGEN_FOUND)
