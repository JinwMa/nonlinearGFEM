set(SOURCES input.cpp)

add_library(InputLib SHARED ${SOURCES})

target_include_directories(InputLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})