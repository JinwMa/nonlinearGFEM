# cmake 的固定部分
cmake_minimum_required(VERSION 3.13)
project(M3D)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Current operating system: ${CMAKE_SYSTEM_NAME}")

# 设置Eigen路径
set(EIGEN3_INCLUDE_DIR "E:/work/project/workspace/eigen-3.4.0")
# 设置MKL路径
set(MKLROOT "C:/Program Files (x86)/Intel/oneAPI/mkl/2023.2.0")
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(STATUS "The operating system is Linux")
    set(MKLROOT "/home/ma/intel/oneapi/mkl/latest")
    set(EIGEN3_INCLUDE_DIR "/home/ma/work/workspace/eigen-3.4.0")
endif()
# 添加包含目录
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${EIGEN3_INCLUDE_DIR} ${MKLROOT}/include)
# 设置MKL库目录
link_directories(${MKLROOT}/lib/intel64)
#添加MKL相关定义 加速计算
add_definitions(-DEIGEN_USE_MKL_ALL)
find_package(OpenMP REQUIRED)
# 指定可执行文件的名称和源文件
add_executable(M3D source/main.cpp  source/test.cpp)
target_link_libraries(M3D PRIVATE OpenMP::OpenMP_CXX)
# 链接MKL库
target_link_libraries(M3D PRIVATE mkl_intel_lp64 mkl_sequential mkl_core mkl_intel_thread)


add_subdirectory(source/element)
target_link_libraries(M3D PRIVATE mylib)

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    # set(CMAKE_BUILD_TYPE "Debug")
endif()

# 设置编译器标志以优化执行速度
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto -fno-fat-lto-objects -funroll-loops -ffast-math")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g -march=native -flto -fno-fat-lto-objects -funroll-loops -ffast-math")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -flto")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /DNDEBUG /GL /arch:AVX2 /permissive- /sdl /Zc:inline /Zc:wchar_t /Zc:forScope /Oi /MD")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /O2 /Ob2 /DNDEBUG /GL /Zi /DEBUG /arch:AVX2 /permissive- /sdl /Zc:inline /Zc:wchar_t /Zc:forScope /Oi /MD")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /LTCG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /DEBUG /Od")
endif()

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    target_compile_options(M3D PRIVATE -g -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(M3D PRIVATE -fsanitize=address -fno-omit-frame-pointer)
endif()

# 指定可执行文件的输出目录(可选)
set_target_properties(M3D PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)


