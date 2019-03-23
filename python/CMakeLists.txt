cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)

find_package(PythonLibs 3.6 REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
set(BOOST_ROOT /boost64/boost167_py36)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})

link_libraries(${PYTHON_LIBRARIES})
python_add_module(graph graph.cpp)
