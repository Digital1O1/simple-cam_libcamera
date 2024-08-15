# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/digital101/simple-cam_libcamera/SignalAndSlotExample

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/digital101/simple-cam_libcamera/SignalAndSlotExample/build

# Include any dependencies generated for this target.
include CMakeFiles/SignalAndSlotExample.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/SignalAndSlotExample.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/SignalAndSlotExample.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SignalAndSlotExample.dir/flags.make

CMakeFiles/SignalAndSlotExample.dir/main.cpp.o: CMakeFiles/SignalAndSlotExample.dir/flags.make
CMakeFiles/SignalAndSlotExample.dir/main.cpp.o: /home/digital101/simple-cam_libcamera/SignalAndSlotExample/main.cpp
CMakeFiles/SignalAndSlotExample.dir/main.cpp.o: CMakeFiles/SignalAndSlotExample.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/digital101/simple-cam_libcamera/SignalAndSlotExample/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/SignalAndSlotExample.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SignalAndSlotExample.dir/main.cpp.o -MF CMakeFiles/SignalAndSlotExample.dir/main.cpp.o.d -o CMakeFiles/SignalAndSlotExample.dir/main.cpp.o -c /home/digital101/simple-cam_libcamera/SignalAndSlotExample/main.cpp

CMakeFiles/SignalAndSlotExample.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SignalAndSlotExample.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/digital101/simple-cam_libcamera/SignalAndSlotExample/main.cpp > CMakeFiles/SignalAndSlotExample.dir/main.cpp.i

CMakeFiles/SignalAndSlotExample.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SignalAndSlotExample.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/digital101/simple-cam_libcamera/SignalAndSlotExample/main.cpp -o CMakeFiles/SignalAndSlotExample.dir/main.cpp.s

# Object files for target SignalAndSlotExample
SignalAndSlotExample_OBJECTS = \
"CMakeFiles/SignalAndSlotExample.dir/main.cpp.o"

# External object files for target SignalAndSlotExample
SignalAndSlotExample_EXTERNAL_OBJECTS =

SignalAndSlotExample: CMakeFiles/SignalAndSlotExample.dir/main.cpp.o
SignalAndSlotExample: CMakeFiles/SignalAndSlotExample.dir/build.make
SignalAndSlotExample: /usr/lib/x86_64-linux-gnu/libQt6Widgets.so.6.4.2
SignalAndSlotExample: /usr/lib/x86_64-linux-gnu/libQt6Gui.so.6.4.2
SignalAndSlotExample: /usr/lib/x86_64-linux-gnu/libQt6Core.so.6.4.2
SignalAndSlotExample: /usr/lib/x86_64-linux-gnu/libGLX.so
SignalAndSlotExample: /usr/lib/x86_64-linux-gnu/libOpenGL.so
SignalAndSlotExample: CMakeFiles/SignalAndSlotExample.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/digital101/simple-cam_libcamera/SignalAndSlotExample/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable SignalAndSlotExample"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SignalAndSlotExample.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SignalAndSlotExample.dir/build: SignalAndSlotExample
.PHONY : CMakeFiles/SignalAndSlotExample.dir/build

CMakeFiles/SignalAndSlotExample.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SignalAndSlotExample.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SignalAndSlotExample.dir/clean

CMakeFiles/SignalAndSlotExample.dir/depend:
	cd /home/digital101/simple-cam_libcamera/SignalAndSlotExample/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/digital101/simple-cam_libcamera/SignalAndSlotExample /home/digital101/simple-cam_libcamera/SignalAndSlotExample /home/digital101/simple-cam_libcamera/SignalAndSlotExample/build /home/digital101/simple-cam_libcamera/SignalAndSlotExample/build /home/digital101/simple-cam_libcamera/SignalAndSlotExample/build/CMakeFiles/SignalAndSlotExample.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SignalAndSlotExample.dir/depend

