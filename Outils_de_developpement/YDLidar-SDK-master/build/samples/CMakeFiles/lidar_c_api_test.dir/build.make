# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_SOURCE_DIR = /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build

# Include any dependencies generated for this target.
include samples/CMakeFiles/lidar_c_api_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include samples/CMakeFiles/lidar_c_api_test.dir/compiler_depend.make

# Include the progress variables for this target.
include samples/CMakeFiles/lidar_c_api_test.dir/progress.make

# Include the compile flags for this target's objects.
include samples/CMakeFiles/lidar_c_api_test.dir/flags.make

samples/CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.o: samples/CMakeFiles/lidar_c_api_test.dir/flags.make
samples/CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.o: /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/samples/lidar_c_api_test.c
samples/CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.o: samples/CMakeFiles/lidar_c_api_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object samples/CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.o"
	cd /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build/samples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT samples/CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.o -MF CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.o.d -o CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.o -c /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/samples/lidar_c_api_test.c

samples/CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.i"
	cd /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build/samples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/samples/lidar_c_api_test.c > CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.i

samples/CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.s"
	cd /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build/samples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/samples/lidar_c_api_test.c -o CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.s

# Object files for target lidar_c_api_test
lidar_c_api_test_OBJECTS = \
"CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.o"

# External object files for target lidar_c_api_test
lidar_c_api_test_EXTERNAL_OBJECTS =

lidar_c_api_test: samples/CMakeFiles/lidar_c_api_test.dir/lidar_c_api_test.c.o
lidar_c_api_test: samples/CMakeFiles/lidar_c_api_test.dir/build.make
lidar_c_api_test: libydlidar_sdk.a
lidar_c_api_test: samples/CMakeFiles/lidar_c_api_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../lidar_c_api_test"
	cd /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build/samples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lidar_c_api_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/CMakeFiles/lidar_c_api_test.dir/build: lidar_c_api_test
.PHONY : samples/CMakeFiles/lidar_c_api_test.dir/build

samples/CMakeFiles/lidar_c_api_test.dir/clean:
	cd /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build/samples && $(CMAKE_COMMAND) -P CMakeFiles/lidar_c_api_test.dir/cmake_clean.cmake
.PHONY : samples/CMakeFiles/lidar_c_api_test.dir/clean

samples/CMakeFiles/lidar_c_api_test.dir/depend:
	cd /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/samples /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build/samples /home/oliver/Documents/3A/2425_ESE_Projet_V-NOM/Outils_de_developpement/YDLidar-SDK-master/build/samples/CMakeFiles/lidar_c_api_test.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : samples/CMakeFiles/lidar_c_api_test.dir/depend

