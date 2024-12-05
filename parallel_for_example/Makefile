# Variables
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -Winvalid-pch -Wnon-virtual-dtor -Wextra -Werror -pg -Wno-unused-parameter

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
           -I/usr/include \
           -I/usr/local/include/opencv4

# Libraries
LIBS = $(LIBCAMERA_LIBS) $(LIBEVENT_LIBS) $(OPENCV_LIBS) -ltbb

# Source files
SRCS = main.cpp

# Output executable
TARGET = parallelTesting

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Clean up
clean:
	rm -f $(TARGET)

.PHONY: all clean
