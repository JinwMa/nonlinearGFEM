
# set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# # set(SOURCES mesh.cpp)

# add_library(PostLib SHARED ${SOURCES})

# target_include_directories(PostLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_link_libraries(PostLib PRIVATE InputLib MeshLib ObjectLib)

cmake_minimum_required(VERSION 3.13)
project(PostLib)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# 指定静态库的名称和包含的源文件
add_library(postlib STATIC ${SOURCES})
target_include_directories(postlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_include_directories(postlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_include_directories(postlib PUBLIC "E:/work/project/workspace/source/object")

# target_include_directories(postlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../object)
# message("XXXXXXXXXXXX  ${CMAKE_CURRENT_SOURCE_DIR}")


target_link_libraries(postlib PRIVATE inputlib meshlib objectlib toolboxlib)

# 指定静态库的输出目录(可选)
set_target_properties(postlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)
