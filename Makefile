# Variables
CXX = g++
# CXXFLAGS = -std=c++17 -Wall -g -Winvalid-pch -Wnon-virtual-dtor -Wextra -Werror -Wno-unused-parameter
CXXFLAGS = -std=c++17 -Wall -g -Winvalid-pch -Wnon-virtual-dtor -Wextra -Wno-unused-parameter

PKG_CONFIG = pkg-config

LIBCAMERA_CFLAGS = $(shell $(PKG_CONFIG) --cflags libcamera)
LIBCAMERA_LIBS = $(shell $(PKG_CONFIG) --libs libcamera --libs opencv4)

LIBEVENT_CFLAGS = $(shell $(PKG_CONFIG) --cflags libevent_pthreads)
LIBEVENT_LIBS = $(shell $(PKG_CONFIG) --libs libevent_pthreads)

# Include directories
INCLUDES = -I. $(LIBCAMERA_CFLAGS) $(LIBEVENT_CFLAGS) \
           -I/usr/local/include/opencv4 \
           -I/home/pi/opencv/3rdparty/openjpeg/openjp2 \
           -I/usr/include/rpicam-apps/core \
           -I/home/pi/libcamera/include/linux \
           -I/usr/include/rpicam-apps/core \
           -I/home/pi/rpicam-apps/core \
           -I/usr/include

# Libraries
LIBS = $(LIBCAMERA_LIBS) $(LIBEVENT_LIBS) 

# Source files
SRCS = simple-cam.cpp event_loop.cpp

# Output executable
TARGET = simple-cam

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Clean up
clean:
	rm -f $(TARGET)

.PHONY: all clean
