
# set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# # set(SOURCES mesh.cpp)

# add_library(LoadLib SHARED ${SOURCES})

# target_include_directories(LoadLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_link_libraries(LoadLib PRIVATE InputLib MeshLib)


cmake_minimum_required(VERSION 3.13)
project(LoadLib)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# 指定静态库的名称和包含的源文件
add_library(loadlib STATIC ${SOURCES})
target_include_directories(loadlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(loadlib PRIVATE inputlib meshlib)

# 指定静态库的输出目录（可选）
set_target_properties(loadlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)