cmake_minimum_required(VERSION 3.13)
project(MyLib)

# 指定静态库的名称和包含的源文件
add_library(mylib STATIC Linear_Hex8.cpp)

# 指定静态库的输出目录(可选)
set_target_properties(mylib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)