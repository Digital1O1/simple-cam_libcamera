cmake_minimum_required(VERSION 3.16)

project(SignalAndSlotExample)

# Find the required Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Create a list of source files
set(SOURCES
    main.cpp
)

# Add the main executable
add_executable(SignalAndSlotExample ${SOURCES})

# Link the Qt6 Widgets library
target_link_libraries(SignalAndSlotExample Qt6::Widgets)

# Enable automatic MOC processing
set(CMAKE_AUTOMOC ON)
