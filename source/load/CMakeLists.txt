
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# set(SOURCES mesh.cpp)

add_library(LoadLib SHARED ${SOURCES})

target_include_directories(LoadLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(LoadLib PRIVATE InputLib MeshLib)