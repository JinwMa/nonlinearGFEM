
# set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# # set(SOURCES mesh.cpp)

# add_library(SolverLib SHARED ${SOURCES})

# target_include_directories(SolverLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_link_libraries(SolverLib PRIVATE InputLib MeshLib PostLib ObjectLib)


cmake_minimum_required(VERSION 3.13)
project(SolverLib)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# 指定静态库的名称和包含的源文件
add_library(solverlib STATIC ${SOURCES})
target_include_directories(solverlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(solverlib PRIVATE toolboxlib elementlib inputlib meshlib postlib objectlib constraintlib loadlib)
target_link_libraries(solverlib PRIVATE OpenMP::OpenMP_CXX)


# 指定静态库的输出目录(可选)
set_target_properties(solverlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE}"
)
