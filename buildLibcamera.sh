#!/bin/bash

# Update and install essential packages
sudo apt-get update
sudo apt-get install -y \
    g++ \
    clang \
    meson \
    ninja-build \
    pkg-config \
    libyaml-dev \
    python3-yaml \
    openssl \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libpython3-dev \
    pybind11-dev 

# Clone the libcamera repository
git clone https://git.libcamera.org/libcamera/libcamera.git

# Navigate into the libcamera directory
cd libcamera

# Setup and build libcamera
meson setup build
ninja -C build install

echo "libcamera has been built and installed successfully."
