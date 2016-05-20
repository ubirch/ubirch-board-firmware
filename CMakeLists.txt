cmake_minimum_required(VERSION 3.0.2)
if (NOT CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE ../ubirch-arm-toolchain/cmake/ubirch-arm-gcc-toolchain.cmake)
endif ()

# set board type (ubirch1r02, FRDM-K82F, FRDM-KL82Z*)
# (* not yet implemented)
# you don't need to set it here, use 'cmake -DBOARD=<board> ..' during setup
# the board setting will determine the used MCU
if (NOT BOARD)
  set(BOARD "ubirch1r02")
  #  set(BOARD "FRDM-K82F")
  #  set(BOARD "FRDM-KL82Z")
endif ()
message(STATUS "selected board: ${BOARD}")

# we set the used MCU based on the board
if (BOARD MATCHES "ubirch1|FRDM-K82F")
  set(MCU "MK82F25615")
elseif (BOARD MATCHES "FRDM-KL82Z")
  set(MCU "MKL82Z7")
endif ()

project(ubirch-firmware C CXX ASM)

require(PACKAGE KinetisSDK MCU ${MCU} VERSION 2.0)
require(PACKAGE wolfSSL MCU ${MCU} VERSION 3.9.0)

# board specific code (pins, peripherals etc.)
if (BOARD MATCHES "ubirch1r02")
  set(BOARD_DIR ubirch1r02)
elseif (BOARD MATCHES "FRDM-K82F")
  set(BOARD_DIR frdm_k82f)
elseif (BOARD MATCHES "FRDM-KL82Z")
  set(BOARD_DIR frdm_kl82z)
else ()
  message(FATAL_ERROR "Please set a board type: -DBOARD=<...> (i.e. ubirch1r02, FRDM-K82F, FRDM-KL82Z)")
endif ()
add_subdirectory(board/${BOARD_DIR})

# ubirch driver code
# we need to add the include directory here, so it does not get
# lost in the nested scope of add_subdirectory()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/driver)
add_subdirectory(firmware)

find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif (DOXYGEN_FOUND)

provide(PACKAGE ubirch VERSION 1.0 BOARD ${BOARD} TARGETS board firmware)
