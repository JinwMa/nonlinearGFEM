# cmake 的固定部分
cmake_minimum_required(VERSION 3.13)
project(M3D)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")
#  
message(STATUS "Current operating system: ${CMAKE_SYSTEM_NAME}")
add_definitions(-DNDIM=3)

###1111111111111111111111111111111111111111111111111111111111111111
# 设置默认构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    # 可选：设置为 Debug 模式进行开发
    # set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

###111111111111111111111111111111111111111111111111111111111111111


###222222222222222222222222222222222222222222222222222222222222222
# 检查编译器标志支持的函数
include(CheckCXXCompilerFlag)
function(check_and_add_compiler_flag flag result_var)
    check_cxx_compiler_flag(${flag} ${result_var})
    if(${result_var})
        message(STATUS "Compiler supports ${flag}")
    else()
        message(STATUS "Compiler does NOT support ${flag}")
    endif()
endfunction()
###2222222222222222222222222222222222222222222222222222222222222222


###3333333333333333333333333333333333333333333333333333333333333333
# 编译器特定的优化设置
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # 检查支持的编译标志
    check_and_add_compiler_flag("-march=native" HAS_MARCH_NATIVE)
    check_and_add_compiler_flag("-flto" HAS_FLTO)
    check_and_add_compiler_flag("-ffast-math" HAS_FFAST_MATH)
    check_and_add_compiler_flag("-funroll-loops" HAS_FUNROLL_LOOPS)    
    # Release 模式：有条件地添加支持的标志
    if(HAS_MARCH_NATIVE)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
    endif()
    if(HAS_FLTO)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
    endif()
    if(HAS_FFAST_MATH)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math")
    endif()
    if(HAS_FUNROLL_LOOPS)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops")
    endif()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fno-fat-lto-objects")
    
    # RelWithDebInfo 模式
    if(HAS_MARCH_NATIVE)
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -march=native")
    endif()
    if(HAS_FLTO)
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -flto")
        set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -flto")
    endif()
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g -fno-fat-lto-objects")
    
    # Debug 模式
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fno-omit-frame-pointer")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC 优化设置（保持不变）
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /DNDEBUG /GL /arch:AVX2 /permissive- /sdl /Zc:inline /Zc:wchar_t /Zc:forScope /Oi /MD")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /O2 /Ob2 /DNDEBUG /GL /Zi /DEBUG /arch:AVX2 /permissive- /sdl /Zc:inline /Zc:wchar_t /Zc:forScope /Oi /MD")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /LTCG")
    
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /DEBUG /Od /RTC1")
endif()
###3333333333333333333333333333333333333333333333333333333333333333

###4444444444444444444444444444444444444444444444444444444444444444
add_executable(M3D source/main.cpp source/solve.cpp source/test.cpp)
###4444444444444444444444444444444444444444444444444444444444444444


###5555555555555555555555555555555555555555555555555555555555555555
# Debug 模式下的额外检测工具
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Enabling advanced debugging tools for Debug build")
        
        # 地址消毒剂（内存错误检测）
        check_cxx_compiler_flag("-fsanitize=address" HAS_ASAN)
        if(HAS_ASAN)
            target_compile_options(M3D PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(M3D PRIVATE -fsanitize=address)
            message(STATUS "  → Address Sanitizer: ENABLED")
        else()
            message(STATUS "  → Address Sanitizer: NOT SUPPORTED")
        endif()
        
        # 未定义行为检测
        check_cxx_compiler_flag("-fsanitize=undefined" HAS_UBSAN)
        if(HAS_UBSAN)
            target_compile_options(M3D PRIVATE -fsanitize=undefined)
            target_link_options(M3D PRIVATE -fsanitize=undefined)
            message(STATUS "  → Undefined Behavior Sanitizer: ENABLED")
        endif()
        
        # 额外的调试信息
        target_compile_options(M3D PRIVATE -g3 -Og)  # 最大调试信息，轻度优化
        
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # MSVC 的调试工具
        target_compile_options(M3D PRIVATE /Zi /DEBUG /RTC1 /sdl)
        message(STATUS "  → MSVC Runtime Checks: ENABLED")
        message(STATUS "  → Security Development Lifecycle: ENABLED")
    endif()
    
    # 所有编译器通用的调试设置
    target_compile_definitions(M3D PRIVATE DEBUG=1 _DEBUG=1)
    message(STATUS "  → Debug symbols: ENABLED")
    
endif()
###5555555555555555555555555555555555555555555555555555555555555555


###6666666666666666666666666666666666666666666666666666666666666666
# 添加OpenMP支持
  find_package(OpenMP REQUIRED)
  target_link_libraries(M3D PUBLIC OpenMP::OpenMP_CXX)



# 添加Eigen支持
  set(EIGEN3_INCLUDE_DIR "E:/work/project/workspace/eigen-3.4.0")
  if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
      message(STATUS "The operating system is Linux")
      ###########################################################
      #笔记本需要修改下面这两行
      ###########################################################
      set(EIGEN3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/eigen-3.4.0")
  endif()
  # 添加包含目录
  include_directories(${EIGEN3_INCLUDE_DIR})

  
# 添加MKL支持
  find_package(MKL REQUIRED)
  add_definitions(-DEIGEN_USE_MKL_ALL)
  target_compile_definitions(M3D PRIVATE EIGEN_USE_MKL_ALL)
  #添加MKL相关定义 加速计算
  #添加包含目录
  include_directories(${MKL_INCLUDE})
  #链接 MKL 库
  target_link_libraries(M3D PRIVATE MKL::MKL)
  # 链接MKL库 -- 手动链接
  # target_link_libraries(M3D PRIVATE mkl_intel_lp64 mkl_core mkl_intel_thread iomp5)
  # target_link_libraries(M3D PRIVATE  pthread dl m)


  # MKL debug
     #message("###############")
     #message("${MKL_INCLUDE}")
     #message("${MKL_LIBRARIES}")
     #message("###############")
  # 检查所有 MKL 相关变量
     #get_cmake_property(_variableNames VARIABLES)
     #list(FILTER _variableNames INCLUDE REGEX "^MKL")
     #foreach(_variableName ${_variableNames})
     #    message(STATUS "${_variableName}=${${_variableName}}")
     #endforeach()


# 添加编译器标志，启用 16 字节或 32 字节的对齐支持
   if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
       add_compile_options(-DEIGEN_DONT_ALIGN_STATICALLY -march=native)
   elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
       add_compile_options(/DEIGEN_DONT_ALIGN_STATICALLY /arch:AVX)
   endif()

# add_compile_options(-DEIGEN_DONT_ALIGN_STATICALLY) # 避免静态内存对齐问题（适用于某些环境）。
# add_definitions(-DEIGEN_MAKE_ALIGNED_OPERATOR_NEW)
# add_definitions(-DEIGEN_DEFAULT_TO_ROW_MAJOR)      # 默认使用行优先存储。
# add_definitions(-DEIGEN_DONT_ALIGN_STATICALLY)     # 避免静态对齐问题。
###6666666666666666666666666666666666666666666666666666666666666666


set(SUBDIRECTORIES
    input
    mesh
    post
    object
    solver
    material
    element
    load
    constraint
    pre
    toolbox
)

set(SUB_LIBRARIES)
foreach(subdir ${SUBDIRECTORIES})
    # 检查目录是否存在
    if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/source/${subdir})
        message(STATUS "Adding subdirectory: source/${subdir}")
        add_subdirectory(source/${subdir})
        list(APPEND SUB_LIBRARIES ${subdir}lib)
    else()
        message(WARNING "Subdirectory source/${subdir} does not exist")
    endif()
endforeach()

# 链接所有找到的库
if(SUB_LIBRARIES)
    message(STATUS "Linking libraries: ${SUB_LIBRARIES}")
    target_link_libraries(M3D PRIVATE ${SUB_LIBRARIES})
else()
    message(WARNING "No subdirectory libraries found to link")
endif()

# 自动添加包含目录
foreach(subdir ${SUBDIRECTORIES})
    set(include_dir ${PROJECT_SOURCE_DIR}/source/${subdir})
    if(IS_DIRECTORY ${include_dir})
        target_include_directories(M3D PRIVATE ${include_dir})
    endif()
endforeach()


# 指定可执行文件的输出目录(可选)
set_target_properties(M3D PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)


