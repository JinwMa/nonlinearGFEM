
# set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# # set(SOURCES mesh.cpp)

# add_library(MeshLib SHARED ${SOURCES})

# target_include_directories(MeshLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_link_libraries(MeshLib PRIVATE InputLib)




cmake_minimum_required(VERSION 3.13)
project(MeshLib)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# 指定静态库的名称和包含的源文件
add_library(meshlib STATIC ${SOURCES})
target_include_directories(meshlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(meshlib PRIVATE inputlib toolboxlib)

# 指定静态库的输出目录(可选)
set_target_properties(meshlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)