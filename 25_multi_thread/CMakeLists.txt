add_executable(hello_thread hello_thread.c )
target_link_libraries(hello_thread gtp)


add_executable(thread01 thread01.c echo.c)
target_link_libraries(thread01 gtp)


add_executable(thread02 thread02.c echo.c)
target_link_libraries(thread02 gtp)
