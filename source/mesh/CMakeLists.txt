
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# set(SOURCES mesh.cpp)

add_library(MeshLib SHARED ${SOURCES})

target_include_directories(MeshLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(MeshLib PRIVATE InputLib)