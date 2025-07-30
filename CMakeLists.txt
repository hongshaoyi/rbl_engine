cmake_minimum_required(VERSION 3.25.1)

project(rbl_engine LANGUAGES CXX)

#默认是Debug版本
#set(CMAKE_BUILD_TYPE "Release")

# 确保头文件修改触发重编译
set(CMAKE_DEPENDS_USE_COMPILER TRUE)

option(IS_OPEN_TEST "是否编译单元测试模块，Debug下默认开启，Release下默认关闭" ON)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(IS_OPEN_TEST OFF)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(src)
add_subdirectory(third_party)

if(IS_OPEN_TEST)
    enable_testing()

    add_subdirectory(test)
endif()