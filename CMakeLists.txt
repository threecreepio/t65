cmake_minimum_required(VERSION 3.10)

# set the project name
project(t65sim)
set (CMAKE_CXX_FLAGS "-m32")

# add the executable
add_executable(t65sim main.c)

add_custom_command(TARGET t65sim POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/t65 $<TARGET_FILE_DIR:t65sim>)
add_custom_command(TARGET t65sim POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/t65.bat $<TARGET_FILE_DIR:t65sim>)
