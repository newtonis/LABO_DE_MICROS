# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2019.1.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2019.1.2\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\joa-m\LABO_DE_MICROS\DecodeBits

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\joa-m\LABO_DE_MICROS\DecodeBits\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/DecodeBits.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/DecodeBits.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/DecodeBits.dir/flags.make

CMakeFiles/DecodeBits.dir/main.cpp.obj: CMakeFiles/DecodeBits.dir/flags.make
CMakeFiles/DecodeBits.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\joa-m\LABO_DE_MICROS\DecodeBits\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/DecodeBits.dir/main.cpp.obj"
	C:\MinGW\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\DecodeBits.dir\main.cpp.obj -c C:\Users\joa-m\LABO_DE_MICROS\DecodeBits\main.cpp

CMakeFiles/DecodeBits.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DecodeBits.dir/main.cpp.i"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\joa-m\LABO_DE_MICROS\DecodeBits\main.cpp > CMakeFiles\DecodeBits.dir\main.cpp.i

CMakeFiles/DecodeBits.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DecodeBits.dir/main.cpp.s"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\joa-m\LABO_DE_MICROS\DecodeBits\main.cpp -o CMakeFiles\DecodeBits.dir\main.cpp.s

# Object files for target DecodeBits
DecodeBits_OBJECTS = \
"CMakeFiles/DecodeBits.dir/main.cpp.obj"

# External object files for target DecodeBits
DecodeBits_EXTERNAL_OBJECTS =

DecodeBits.exe: CMakeFiles/DecodeBits.dir/main.cpp.obj
DecodeBits.exe: CMakeFiles/DecodeBits.dir/build.make
DecodeBits.exe: CMakeFiles/DecodeBits.dir/linklibs.rsp
DecodeBits.exe: CMakeFiles/DecodeBits.dir/objects1.rsp
DecodeBits.exe: CMakeFiles/DecodeBits.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\joa-m\LABO_DE_MICROS\DecodeBits\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable DecodeBits.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\DecodeBits.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/DecodeBits.dir/build: DecodeBits.exe

.PHONY : CMakeFiles/DecodeBits.dir/build

CMakeFiles/DecodeBits.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\DecodeBits.dir\cmake_clean.cmake
.PHONY : CMakeFiles/DecodeBits.dir/clean

CMakeFiles/DecodeBits.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\joa-m\LABO_DE_MICROS\DecodeBits C:\Users\joa-m\LABO_DE_MICROS\DecodeBits C:\Users\joa-m\LABO_DE_MICROS\DecodeBits\cmake-build-debug C:\Users\joa-m\LABO_DE_MICROS\DecodeBits\cmake-build-debug C:\Users\joa-m\LABO_DE_MICROS\DecodeBits\cmake-build-debug\CMakeFiles\DecodeBits.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/DecodeBits.dir/depend
