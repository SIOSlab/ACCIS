cmake_minimum_required(VERSION 3.10)

enable_language(Fortran)

#these options may cause issues with gfortran newer than in gcc8
#either remove -Wall -pedantic or replace with -std=legacy
add_compile_options(-Wall -g -Og -pedantic)

find_package(OpenCV REQUIRED)
find_package(OpenSSL REQUIRED)

project(ACCIS)


include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(.)
include_directories(roam)
#add Eigen and OpenCV include paths here if not installed in standard locations

FILE(GLOB ACCIS_SRC *.cpp)
FILE(GLOB ROAM_SRC roam/*.f)

add_executable(accis ${ACCIS_SRC} ${ROAM_SRC})

set(accis CMAKE_CXX_STANDARD 11)
set(accis CMAKE_CXX_STANDARD_REQUIRED ON)

set(accis CMAKE_FORTRAN_STANDARD 77)
set(accis CMAKE_FORTRAN_STANDARD_REQUIRED ON)

target_link_libraries(accis ${OpenCV_LIBS} OpenSSL::SSL)
