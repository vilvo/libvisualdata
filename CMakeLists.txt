cmake_minimum_required (VERSION 2.6)
project (visualdatasources)
set(CMAKE_CXX_FLAGS "-Wall -D=HAVE_CONFIG_H")

find_package(OpenCV REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} "/usr/local/include")
link_directories(${OpenCV_LIBRARY_DIRS} "/usr/local/lib")

add_executable(test_visualdatasources
  visualdatainterface.cpp
  imagesource.cpp v4l2camthread.cpp idscamthread.cpp adbscreenshotthread.cpp
  test_visualdatasources.cpp
)

target_link_libraries(test_visualdatasources
  ${OpenCV_LIBRARIES} ueye_api boost_thread
  gtest_main gtest
)

