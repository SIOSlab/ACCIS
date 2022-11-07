cmake_minimum_required(VERSION 3.10)

project(accis CXX)

enable_language(Fortran)

set(CMAKE_Fortran_FLAGS "-std=legacy -O3")
set(CMAKE_CXX_FLAGS "-std=c++17 -O3 -Wall -pedantic")

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Java REQUIRED)
find_package(JNI REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PythonLibs REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS})
find_package(pybind11 CONFIG REQUIRED)

include(UseJava)

include_directories(${JNI_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(.)
include_directories(roam)

add_jar(Landsat SOURCES Landsat.java INCLUDE_JARS WorldWindJava.jar)

FILE(GLOB ACCIS_SRC *.cpp)
FILE(GLOB ROAM_SRC roam/*.f)

pybind11_add_module(accis ${ROAM_SRC} ${ACCIS_SRC})

target_link_libraries(accis PRIVATE Eigen3::Eigen)
target_link_libraries(accis PRIVATE ${OpenCV_LIBS})
target_link_libraries(accis PRIVATE ${JNI_LIBRARIES})
target_link_libraries(accis PRIVATE ${PYTHON_LIBRARIES})

set_target_properties(accis PROPERTIES PREFIX "")
set_target_properties(accis PROPERTIES OUTPUT_NAME "accis")
set_target_properties(accis PROPERTIES SUFFIX ".so")
