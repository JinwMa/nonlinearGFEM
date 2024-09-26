cmake_minimum_required(VERSION 3.13)
project(PreLib)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# 指定静态库的名称和包含的源文件
add_library(prelib STATIC ${SOURCES})

target_include_directories(prelib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(prelib PRIVATE elementlib)

# 指定静态库的输出目录（可选）
set_target_properties(prelib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)
