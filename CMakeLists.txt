# cmake 的固定部分
cmake_minimum_required(VERSION 3.13)
project(M3D)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)



# 添加mesh子目录,如果还有别的子目录,则继续添加即可,目前先做到手动添加 #默认当前目录为CMakeLists.txt所在目录
add_subdirectory(source/input)
add_subdirectory(source/mesh)
# 指定可执行文件的名称和源文件
add_executable(M3D source/main.cpp)

# 链接子目录下的动态库
target_link_libraries(M3D PRIVATE MeshLib)
target_include_directories(M3D PRIVATE ${PROJECT_SOURCE_DIR}/source/mesh)

target_link_libraries(M3D PRIVATE InputLib)
target_include_directories(M3D PRIVATE ${PROJECT_SOURCE_DIR}/source/input)

# 设置Eigen路径
set(EIGEN3_INCLUDE_DIR "/home/ma/work/M3D-C/eigen-3.4.0")
include_directories(${EIGEN3_INCLUDE_DIR})
# 设置MKL路径
if(NOT DEFINED MKLROOT)
    set(MKLROOT /home/ma/intel/oneapi/mkl/latest)
endif()
# 添加包含目录
include_directories(${EIGEN3_INCLUDE_DIR} ${MKLROOT}/include)
# 设置MKL库目录
link_directories(${MKLROOT}/lib/intel64)
# 链接MKL库
target_link_libraries(M3D PRIVATE mkl_intel_lp64 mkl_sequential mkl_core pthread dl m)

#添加MKL相关定义 加速计算
add_definitions(-DEIGEN_USE_MKL_ALL)

# 链接静态库到可执行文件
# target_link_libraries(MyExecutable ${MKL_INTEL_LP64} ${MKL_SEQUENTIAL} ${MKL_CORE} ${IOMP5})

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
# 设置编译器标志以优化执行速度
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto -fno-fat-lto-objects -funroll-loops -ffast-math")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g -march=native -flto -fno-fat-lto-objects -funroll-loops -ffast-math")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -flto")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /DNDEBUG /GL /arch:AVX2 /permissive- /sdl /Zc:inline /Zc:wchar_t /Zc:forScope /Oi /MD")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /O2 /Ob2 /DNDEBUG /GL /Zi /DEBUG /arch:AVX2 /permissive- /sdl /Zc:inline /Zc:wchar_t /Zc:forScope /Oi /MD")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /LTCG")
endif()

# 指定可执行文件的输出目录(可选)
set_target_properties(M3D PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)


