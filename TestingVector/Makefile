# Variables
CXX = g++
# CXXFLAGS = -std=c++17 -Wall -Winvalid-pch -Wnon-virtual-dtor -Wextra -Werror -Wno-unused-parameter -g
CXXFLAGS = -std=c++17 -Wall -Winvalid-pch -Wnon-virtual-dtor -g

PKG_CONFIG = pkg-config

LIBCAMERA_CFLAGS = $(shell $(PKG_CONFIG) --cflags libcamera)
LIBCAMERA_LIBS = $(shell $(PKG_CONFIG) --libs libcamera --libs opencv4)

LIBEVENT_CFLAGS = $(shell $(PKG_CONFIG) --cflags libevent_pthreads)
LIBEVENT_LIBS = $(shell $(PKG_CONFIG) --libs libevent_pthreads)

INCLUDES = -I. $(LIBCAMERA_CFLAGS) $(LIBEVENT_CFLAGS)  -I/usr/local/include/opencv4
LIBS = $(LIBCAMERA_LIBS) $(LIBEVENT_LIBS)

# Source files
SRCS = main.cpp

# Output executable
TARGET = testVector

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Clean up
clean:
	rm -f $(TARGET)

.PHONY: all clean
