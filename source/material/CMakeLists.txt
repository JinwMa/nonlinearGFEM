
# set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# # set(SOURCES mesh.cpp)

# add_library(ConstraintLib SHARED ${SOURCES})

# target_include_directories(ConstraintLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_link_libraries(ConstraintLib PRIVATE InputLib MeshLib)




cmake_minimum_required(VERSION 3.13)
project(MaterialLib)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# 指定静态库的名称和包含的源文件
add_library(materiallib STATIC ${SOURCES})
target_include_directories(materiallib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(materiallib PRIVATE inputlib meshlib toolboxlib objectlib)

# 指定静态库的输出目录(可选)
set_target_properties(materiallib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)
