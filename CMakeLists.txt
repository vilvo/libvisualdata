/*
 * cmake build file for libvisualdata
 * Copyright (c) 2013-2014, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

project(libvisualdata)
cmake_minimum_required (VERSION 2.6)
set(LIBRARY_OUTPUT_PATH lib)
#set(CMAKE_CXX_FLAGS "-Wall -D=HAVE_CONFIG_H")
find_package(OpenCV REQUIRED)

add_library(libvisualdata SHARED
    visualdatainterface.cpp
    imagesource.cpp
    v4l2camthread.cpp
    idscamthread.cpp
    adbscreenshotthread.cpp
)

target_link_libraries(libvisualdata
    ${OpenCV_LIBRARIES}
    # TODO - build with ueye_api for IDS camera support only if available
    ueye_api
    boost_thread
)

set_target_properties(libvisualdata PROPERTIES OUTPUT_NAME visualdata)
install(TARGETS libvisualdata LIBRARY DESTINATION lib)

include_directories( ${OpenCV_INCLUDE_DIRS} "/usr/local/include")
link_directories(${OpenCV_LIBRARY_DIRS} "/usr/local/lib")

add_executable(test_visualdata
    visualdatainterface.cpp
    imagesource.cpp
    v4l2camthread.cpp
    idscamthread.cpp
    adbscreenshotthread.cpp
    test_visualdatasources.cpp
)

target_link_libraries(test_visualdata
    gtest_main pthread gtest libvisualdata
)

