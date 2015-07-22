cmake_minimum_required(VERSION 2.8.3)
project(rosnect2)
SET(CMAKE_BUILD_TYPE Release)

LIST(APPEND CMAKE_CXX_FLAGS "-std=c++11")

find_package(freenect2 CONFIG)
find_package(PCL REQUIRED)
find_package(catkin REQUIRED COMPONENTS roscpp rostime std_msgs sensor_msgs cv_bridge 
	compressed_depth_image_transport depth_registration)
find_package(cmake_modules QUIET)

## System dependencies are found with CMake's conventions
find_package(OpenCV REQUIRED)

catkin_package(
  INCLUDE_DIRS include
#  LIBRARIES kinect2_bridge
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS system_lib
)

#Jetson patched libusb
LINK_DIRECTORIES("libusb/lib")

include_directories(include
  ${freenect2_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIR}
  ${PCL_INCLUDE_DIRS}
)

add_executable(rosnect2 src/rosnect2.cpp)
target_link_libraries(rosnect2
  /usr/lib/arm-linux-gnueabihf/tegra/libjpeg.so
  usb-1.0
  ${freenect2_LIBRARY}
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
  ${PCL_LIBRARIES}
)

