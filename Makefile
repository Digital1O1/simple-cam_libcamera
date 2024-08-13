# Variables
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -Winvalid-pch -Wnon-virtual-dtor -Wextra -Werror -Wno-unused-parameter

PKG_CONFIG = pkg-config

LIBCAMERA_CFLAGS = $(shell $(PKG_CONFIG) --cflags libcamera)
LIBCAMERA_LIBS = $(shell $(PKG_CONFIG) --libs libcamera)

LIBEVENT_CFLAGS = $(shell $(PKG_CONFIG) --cflags libevent_pthreads)
LIBEVENT_LIBS = $(shell $(PKG_CONFIG) --libs libevent_pthreads)

OPENCV_CFLAGS = $(shell $(PKG_CONFIG) --cflags opencv4)
OPENCV_LIBS = $(shell $(PKG_CONFIG) --libs opencv4)

# Include directories
INCLUDES = -I. $(LIBCAMERA_CFLAGS) $(LIBEVENT_CFLAGS) $(OPENCV_CFLAGS) \
           -I/home/pi/libcamera/include/linux \
           -I/usr/include

# Libraries
LIBS = $(LIBCAMERA_LIBS) $(LIBEVENT_LIBS) $(OPENCV_LIBS)

# Source files
COMMON_SRCS = event_loop.cpp image.cpp
SIMPLE_CAM_SRCS = simple-cam.cpp $(COMMON_SRCS)
DUAL_CAM_SRCS = dual-camera.cpp $(COMMON_SRCS)

# Output executables
TARGET = simple-cam
TARGET2 = dual-cam

# Default target
all: $(TARGET) $(TARGET2)

# Link object files to create executables
$(TARGET): $(SIMPLE_CAM_SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)
# $@ = name of target file
# $  = represents all prerequisites/dependencies of target
$(TARGET2): $(DUAL_CAM_SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Clean up
clean:
	rm -f $(TARGET) $(TARGET2)

.PHONY: all clean
