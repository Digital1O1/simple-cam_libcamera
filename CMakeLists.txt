cmake_minimum_required(VERSION 3.6)

project(SimpleCam
	DESCRIPTION "A small and documented example application for libcamera"
	LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)

# set (CMAKE_CXX_FLAGS "-Wall -Winvalid-pch -Wnon-virtual-dtor -Wextra -Werror -Wno-unused-parameter")
set (CMAKE_CXX_FLAGS "-Wall -Winvalid-pch -Wnon-virtual-dtor -Wextra -Wno-unused-parameter")

find_package(PkgConfig)

find_package(dma-heap)

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)
message(STATUS "libcamera library found:")
message(STATUS "    version: ${LIBCAMERA_VERSION}")
message(STATUS "    libraries: ${LIBCAMERA_LINK_LIBRARIES}")
message(STATUS "    include path: ${LIBCAMERA_INCLUDE_DIRS}")

# libevent is used specifically by simple-cam as its event loop.
# Applications may use a different event handling implementation.
pkg_check_modules(LIBEVENT REQUIRED IMPORTED_TARGET libevent_pthreads)
message(STATUS "libevent_pthreads library found:")
message(STATUS "    version: ${LIBEVENT_VERSION}")
message(STATUS "    libraries: ${LIBEVENT_LINK_LIBRARIES}")
message(STATUS "    include path: ${LIBEVENT_INCLUDE_DIRS}")

find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)
if(OpenCV_FOUND)
  message(STATUS "OpenCV library found: ${OpenCV_FOUND}")
  message(STATUS "    version: ${OpenCV_VERSION}")
  message(STATUS "    libraries: ${OpenCV_LIBS}")
  message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
else()
  message(FATAL_ERROR "OpenCV not found")
endif()


include_directories(${CMAKE_SOURCE_DIR} 
                    ${LIBCAMERA_INCLUDE_DIRS} 
                    ${LIBEVENT_INCLUDE_DIRS} 
                    ${OpenCV_INCLUDE_DIRS} 
                    /home/pi/libcamera/include/linux
					/usr/include/rpicam-apps/core
					/home/pi/rpicam-apps/core)

add_executable(simple-cam simple-cam.cpp event_loop.cpp)

target_link_libraries(simple-cam PkgConfig::LIBEVENT)
target_link_libraries(simple-cam PkgConfig::LIBCAMERA)
target_link_libraries(simple-cam ${OpenCV_LIBS})
