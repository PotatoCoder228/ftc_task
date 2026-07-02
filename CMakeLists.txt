cmake_minimum_required(VERSION 3.10)
project(FTC_task C)

include(CheckIPOSupported)
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(ENABLE_LTO "Enable link time optimization" OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMMON_SOURCES
    src/dumper.c
    src/rbtree.c
)

add_library(common_obj OBJECT ${COMMON_SOURCES})
target_include_directories(common_obj PRIVATE include)

target_compile_options(common_obj PRIVATE -Wall -Wextra -Wpedantic)

add_executable(ftc_dump src/main.c $<TARGET_OBJECTS:common_obj>)

add_executable(ftc_test test/main.c test/test.c $<TARGET_OBJECTS:common_obj>)

target_include_directories(ftc_dump PRIVATE include)
target_include_directories(ftc_test PRIVATE include)

check_ipo_supported(RESULT lto_supported)
if(ENABLE_LTO AND lto_supported)
    set_property(TARGET ftc_dump ftc_test PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(ENABLE_SANITIZERS)
    target_compile_options(common_obj PRIVATE -fsanitize=address -fsanitize=undefined)

    target_link_options(ftc_dump PRIVATE -fsanitize=address -fsanitize=undefined)
    target_link_options(ftc_test PRIVATE -fsanitize=address -fsanitize=undefined)

    message(STATUS "Sanitizers enabled for all targets")
endif()
