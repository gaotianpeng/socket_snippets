add_executable(nonblock_server nonblock_server.c)
target_link_libraries(nonblock_server gtp)


add_executable(nonblock_client nonblock_client.c)
target_link_libraries(nonblock_client gtp)
