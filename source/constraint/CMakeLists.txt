
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# set(SOURCES mesh.cpp)

add_library(ConstraintLib SHARED ${SOURCES})

target_include_directories(ConstraintLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ConstraintLib PRIVATE InputLib MeshLib)