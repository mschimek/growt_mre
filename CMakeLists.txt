
cmake_minimum_required(VERSION 3.10)
project(growt_test)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE RELEASE)

find_package(OpenMP REQUIRED)
find_package(TBB REQUIRED)

add_library(growt INTERFACE)
target_include_directories(growt SYSTEM INTERFACE external/)
target_include_directories(growt SYSTEM INTERFACE external/growt)

add_library(base_target INTERFACE)
target_compile_options(base_target INTERFACE -O3 -march=native)
target_compile_features(base_target INTERFACE cxx_std_17)

target_link_libraries(base_target INTERFACE OpenMP::OpenMP_CXX TBB::tbb)

OPTION(USE_THREAD_SANITIZER "Compile with Thread Sanitizer enabled" OFF)
if (USE_THREAD_SANITIZER)
  target_compile_options(base_target INTERFACE -fsanitize=thread -g -fno-omit-frame-pointer)
  target_link_options(base_target INTERFACE -fsanitize=thread)
endif()

target_link_libraries(base_target INTERFACE growt)

add_executable(main growt.cpp)
target_link_libraries(main PRIVATE base_target growt)


