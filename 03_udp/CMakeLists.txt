add_executable(udp_client udp_client.c)
target_link_libraries(udp_client gtp)

add_executable(udp_server udp_server.c)
target_link_libraries(udp_server gtp)
