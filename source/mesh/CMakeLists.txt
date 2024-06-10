set(SOURCES mesh.cpp)

add_library(MeshLib SHARED ${SOURCES})

target_include_directories(MeshLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(MeshLib PRIVATE InputLib)