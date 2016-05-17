cmake_minimum_required(VERSION 3.0.2)
if (NOT CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE ../ubirch-arm-toolchain/cmake/ubirch-arm-gcc-toolchain.cmake)
endif ()
# set board type (ubirch#1 r0.2, FRDM-K82F, FRDM-KL82Z*)
# (* not yet implemented)
# you don't need to set it here, use 'cmake -DBOARD=<board> ..' during setup
# the board setting will determine the used MCU
if (NOT BOARD)
  #  set(BOARD "ubirch#1")
  set(BOARD "FRDM-K82F")
  #  set(BOARD "FRDM-KL82Z")
endif ()

message(STATUS "selected board: ${BOARD}")
if (BOARD MATCHES "ubirch#1|FRDM-K82F")
  set(MCU "MK82F25615")
elseif (BOARD MATCHES "FRDM-KL82Z")
  set(MCU "MKL82Z7")
endif ()

project(ubirch-firmware C CXX ASM)

require(PACKAGE KinetisSDK MCU ${MCU} VERSION 2.0 BUILD_TYPE MinSizeRel)
require(PACKAGE wolfSSL MCU ${MCU} VERSION 3.9.0 BUILD_TYPE RelWithDebInfo)

add_definitions(-DNCIODEBUG)

# board specific code (pins, peripherals etc.)
add_subdirectory(board)

# ubirch driver code
# we need to add the include directory here, so it does not get
# lost in the nested scope of add_subdirectory()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/driver)
add_subdirectory(firmware)

provide(PACKAGE ubirch-firmware VERSION 1.0 MCU ${MCU} TARGETS board firmware)
