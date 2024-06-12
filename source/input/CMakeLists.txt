
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

add_library(InputLib SHARED ${SOURCES})

target_include_directories(InputLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})