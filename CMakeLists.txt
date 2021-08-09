cmake_minimum_required(VERSION 3.10)

enable_language(Fortran)

add_compile_options(-Wall -g -Og -pedantic)

# Eigen?

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
find_package(OpenCV REQUIRED)
find_package(GDAL REQUIRED)

project(ACCIS)

include(UseJava)

include_directories(${JNI_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${GDAL_INCLUDE_DIRS})
include_directories(.)
include_directories(roam)

add_jar(Landsat SOURCES Landsat.java INCLUDE_JARS WorldWindJava.jar)

FILE(GLOB ACCIS_SRC *.cpp)
FILE(GLOB ROAM_SRC roam/*.f)

add_executable(accis ${ACCIS_SRC} ${ROAM_SRC})

set(accis CMAKE_CXX_STANDARD 11)
set(accis CMAKE_CXX_STANDARD_REQUIRED ON)

set(accis CMAKE_FORTRAN_STANDARD 77)
set(accis CMAKE_FORTRAN_STANDARD_REQUIRED ON)

target_link_libraries(accis ${OpenCV_LIBS})
target_link_libraries(accis ${GDAL_LIBRARIES})
target_link_libraries(accis ${JNI_LIBRARIES})
