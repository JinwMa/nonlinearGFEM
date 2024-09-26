
# set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# # set(SOURCES mesh.cpp)

# add_library(ObjectLib SHARED ${SOURCES})

# target_include_directories(ObjectLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_link_libraries(ObjectLib PRIVATE InputLib MeshLib)




cmake_minimum_required(VERSION 3.13)
project(ObjectLib)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# 指定静态库的名称和包含的源文件
add_library(objectlib STATIC ${SOURCES})

# target_include_directories(objectlib PUBLIC "E:/work/project/workspace/source/mesh")
target_include_directories(objectlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(objectlib PRIVATE inputlib meshlib)

# 指定静态库的输出目录（可选）
set_target_properties(objectlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)
