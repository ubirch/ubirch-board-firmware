# SIM800H (GSM chip) driver
add_library(sim800h sim800h_core.c sim800h_parser.c sim800h_ops.c sim800h_http.c)
target_link_libraries(sim800h board timer dbgutil)
