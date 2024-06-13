
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# set(SOURCES mesh.cpp)

add_library(ObjectLib SHARED ${SOURCES})

target_include_directories(ObjectLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ObjectLib PRIVATE InputLib MeshLib)