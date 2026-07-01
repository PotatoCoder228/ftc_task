cmake_minimum_required(VERSION 3.10)

project(FTC_task C)

include(CheckIPOSupported)
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(ENABLE_LTO "Enable link time optimization" OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(ftc_dump src/main.c src/dumper.c src/rbtree.c)

# -fsanitize=address, -fsanitize=undefined
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")

check_ipo_supported(RESULT res)
if(ENABLE_LTO)
    if(res)
        set_property(TARGET ftc_dump PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

target_compile_options(ftc_dump PRIVATE -Wall -Wextra -Wpedantic)

if(ENABLE_SANITIZERS)
    target_compile_options(ftc_dump PRIVATE 
        -fsanitize=address 
        -fsanitize=undefined
    )
    target_link_options(ftc_dump PRIVATE 
        -fsanitize=address 
        -fsanitize=undefined
    )
    message(STATUS "Sanitizers enabled for ftc_dump")
endif()

target_include_directories(ftc_dump PRIVATE include)