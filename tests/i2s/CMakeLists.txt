add_executable(i2s i2s.c tosdcard.c tosdcard.h)
target_link_libraries(i2s board)
target_link_libraries(i2s firmware)
prepare_flash(TARGET i2s)
#
## copy the config.h.template to config.h in case it is not there; it is ignored by .git!
#if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.h")
#  message(AUTHOR_WARNING "In directory '${CMAKE_CURRENT_SOURCE_DIR}': installing the template config.h, please edit!")
#  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.template ${CMAKE_CURRENT_SOURCE_DIR}/config.h COPYONLY)
#endif ()



