#####################################
cmake_minimum_required (VERSION 2.8)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/lib/cmake/)
project (vision)
find_package(raspicam REQUIRED)
find_package(OpenCV)
IF  ( OpenCV_FOUND AND raspicam_CV_FOUND)
    MESSAGE(STATUS "COMPILING")
    add_executable (vision src/vision.cpp)
    target_link_libraries (vision ${raspicam_CV_LIBS} ${raspicam_LIBS} ${OpenCV_LIBS})
ELSE()
        MESSAGE(FATAL_ERROR "OPENCV NOT FOUND")
ENDIF()
#####################################
