cmake_minimum_required(VERSION 3.6)

project(
  SimpleCam
  DESCRIPTION "Dual-camera libcamera + OpenCV capture example for Raspberry Pi"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile_commands.json for clangd / editor tooling.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS
    "-Wall -Winvalid-pch -Wnon-virtual-dtor -Wextra -Werror -Wno-unused-parameter"
)

# Optional gprof profiling build: cmake -DENABLE_PROFILING=ON ..
# (replaces the old hand-written Makefile's -pg workflow; see projectNotes.md)
option(ENABLE_PROFILING "Build with gprof instrumentation (-pg)" OFF)
if(ENABLE_PROFILING)
  add_compile_options(-pg -g)
  add_link_options(-pg)
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)
message(STATUS "libcamera library found:")
message(STATUS "    version: ${LIBCAMERA_VERSION}")
message(STATUS "    libraries: ${LIBCAMERA_LINK_LIBRARIES}")
message(STATUS "    include path: ${LIBCAMERA_INCLUDE_DIRS}")

# libevent is used specifically by simple-cam as its event loop. Applications
# may use a different event handling implementation.
pkg_check_modules(LIBEVENT REQUIRED IMPORTED_TARGET libevent_pthreads)
message(STATUS "libevent_pthreads library found:")
message(STATUS "    version: ${LIBEVENT_VERSION}")
message(STATUS "    libraries: ${LIBEVENT_LINK_LIBRARIES}")
message(STATUS "    include path: ${LIBEVENT_INCLUDE_DIRS}")

find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)
message(STATUS "OpenCV library found: ${OpenCV_FOUND}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS}
                    ${LIBEVENT_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

# Code shared by both the single- and dual-camera programs.
set(COMMON_SRCS event_loop.cpp image.cpp)

# single-camera: the original, fully commented single-camera example.
add_executable(single-camera simple-cam.cpp ${COMMON_SRCS})

# dual-camera: the main program — captures two cameras and shows them side by side.
add_executable(dual-camera dual-camera.cpp ${COMMON_SRCS})

foreach(target single-camera dual-camera)
  target_link_libraries(${target} PkgConfig::LIBEVENT)
  target_link_libraries(${target} PkgConfig::LIBCAMERA)
  target_link_libraries(${target} ${OpenCV_LIBS})
endforeach()
