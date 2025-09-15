cmake_minimum_required(VERSION 3.13)
project(DiscretizationLib)

# 递归查找所有子目录中的.cpp文件
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

# 递归查找所有子目录中的头文件
file(GLOB_RECURSE HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# 指定静态库的名称和包含的源文件
# add_library(discretizationlib STATIC ${SOURCES})
# 如果你希望IDE也能显示头文件，可以将它们添加到源文件列表中
add_library(discretizationlib STATIC ${SOURCES} ${HEADERS})
# 查找所有包含头文件的目录（包括子目录）
file(GLOB_RECURSE ALL_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
# 获取所有头文件所在的唯一目录
set(HEADER_DIRS "")
foreach(header_file ${ALL_HEADER_FILES})
    get_filename_component(header_dir ${header_file} DIRECTORY)
    list(APPEND HEADER_DIRS ${header_dir})
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)

# 将所有包含头文件的目录都添加到包含路径中
target_include_directories(discretizationlib PUBLIC 
    ${HEADER_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)



target_link_libraries(discretizationlib)

# 指定静态库的输出目录(可选)
set_target_properties(discretizationlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)