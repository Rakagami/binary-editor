cmake_minimum_required(VERSION 3.0)

#---------- WHAT TO CONFIG ------------#
set(PROJECT_NAME "binary-editor")
set(BINARY_NAME "binedit")
#--------------------------------------#

project (${PROJECT_NAME})
# Shared libraries:
set(PROJECT_LINK_LIBS libncurses.so)
link_directories( /usr/lib )

include_directories(src /usr/include)
set(SOURCES src/main.cpp)

add_executable(${BINARY_NAME} ${SOURCES})
target_link_libraries(${BINARY_NAME} ${PROJECT_LINK_LIBS})

target_compile_options(${BINARY_NAME} PUBLIC -Wall -Wpedantic)

add_custom_target(run
    COMMAND ${BINARY_NAME}
    DEPENDS ${BINARY_NAME}
    WORKING_DIRECTORY . 
    COMMENT "Executing the binary"
)
