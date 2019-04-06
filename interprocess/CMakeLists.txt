cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(BOOST_ROOT /boost64/boost167_py36)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
add_executable(mmap_file mmap_file.cpp)
