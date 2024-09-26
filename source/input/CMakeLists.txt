
# set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# add_library(InputLib SHARED ${SOURCES})

# target_include_directories(InputLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


cmake_minimum_required(VERSION 3.13)
project(InputLib)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# 指定静态库的名称和包含的源文件
add_library(inputlib STATIC ${SOURCES})
target_include_directories(inputlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 指定静态库的输出目录（可选）
set_target_properties(inputlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)

# message("${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}")
# message("${CMAKE_CURRENT_SOURCE_DIR}")