cmake_minimum_required(VERSION 3.1.0)
project (6073NES)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/cmake)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

# executable
file(GLOB SOURCES "src/*.cxx")
add_executable(nes ${SOURCES})

# SDL2
target_link_libraries(nes ${SDL2_LIBRARY})

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#add_custom_target(run
#    COMMAND binary
#    DEPENDS binary
#    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
#)
