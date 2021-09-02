cmake_minimum_required(VERSION 3.10.0)
project(ThreadPool VERSION 0.1.0)

# C++ version
#--------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#--------------------------------------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Compiler flags
#--------------------------------------
if(MSVC)
    # Warning level
    add_compile_options(/W3)
    # add_compile_options(/wd4100)    # unused parameter
    # add_compile_options(/wd4324)    # structure was padded due to alignment specifier
    # add_compile_options(/wd4201)    # nonstandard extension used: nameless struct/union
    add_compile_options(/wd4244)    # 'conversion' conversion from 'type1' to 'type2', possible loss of data
    add_compile_options(/wd26451)   # arithmetic overflow
    # add_compile_options(/wd26812)   # arithmetic overflow
    # Security check
    add_compile_options(/GS)
    # Function level linking
    add_compile_options(/Gy)
    # Exceptions
    add_compile_options(/EHsc)
    # SDL checks 2015+
    if(MSVC_VERSION GREATER_EQUAL 1900)
        add_compile_options(/sdl)
    endif()
    add_compile_options(/fp:fast)
    # Runtime library
    add_compile_options("$<IF:$<CONFIG:Debug>,/MDd,/MD>")
    # Program database for edit and continue
    add_compile_options("$<IF:$<CONFIG:Debug>,/ZI,/Zi>")
    # Optimizations
    add_compile_options("$<IF:$<CONFIG:Debug>,/Od,/O2>")
    # Inline function expansion
    add_compile_options("$<IF:$<CONFIG:Debug>,/Ob0,/Ob2>")
    # Basic runtime checks
    add_compile_options("$<$<CONFIG:Debug>:/RTC1>")
    # Enable intrinsics
    add_compile_options("$<$<CONFIG:RELEASE>:/Oi>")
else()
    add_compile_options("$<$<CONFIG:Debug>:-g>")
    add_compile_options("$<IF:$<CONFIG:Debug>,-O0,-O2>")
    add_compile_options(-Wall -Wextra)
    add_compile_options(-Wno-switch -Wno-unused-function -Wno-unused-parameter)
    add_compile_options(-ffast-math)
    if (UNIX)
        add_compile_options(-pthread)
        link_libraries(-lpthread)
    endif()
endif()

# Avoid RelWithDebInfo and MinSizeRel
#--------------------------------------
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Define Release by default
#--------------------------------------
message("CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "Build type not specified: Use Release by default")
endif(NOT CMAKE_BUILD_TYPE)

# Pool
#--------------------------------------
set(SRC_POOL
    src/main.cpp
    src/thread_pool/thread_pool.h
    src/thread_pool/unbounded_queue.h
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${SRC_POOL})

add_executable(pool
    ${SRC_POOL}
)
