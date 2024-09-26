cmake_minimum_required(VERSION 3.13)
project(ElementLib)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# 指定静态库的名称和包含的源文件
add_library(elementlib STATIC ${SOURCES})
target_include_directories(elementlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 指定静态库的输出目录（可选）
set_target_properties(elementlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)

# message("${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}")
# message("${CMAKE_CURRENT_SOURCE_DIR}")