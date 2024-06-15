
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# set(SOURCES mesh.cpp)

add_library(SolverLib SHARED ${SOURCES})

target_include_directories(SolverLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SolverLib PRIVATE InputLib MeshLib)