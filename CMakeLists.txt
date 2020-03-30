cmake_minimum_required(VERSION 3.13)
project(GSoC-submission CXX)
find_package(HPX REQUIRED)
hpx_add_executable(matrixmul matrix_multiplication.cpp)
