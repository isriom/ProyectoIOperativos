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
CMAKE_SOURCE_DIR = /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos

# Include any dependencies generated for this target.
include CMakeFiles/Creator.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Creator.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Creator.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Creator.dir/flags.make

CMakeFiles/Creator.dir/Creator.c.o: CMakeFiles/Creator.dir/flags.make
CMakeFiles/Creator.dir/Creator.c.o: Creator.c
CMakeFiles/Creator.dir/Creator.c.o: CMakeFiles/Creator.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Creator.dir/Creator.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/Creator.dir/Creator.c.o -MF CMakeFiles/Creator.dir/Creator.c.o.d -o CMakeFiles/Creator.dir/Creator.c.o -c /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos/Creator.c

CMakeFiles/Creator.dir/Creator.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/Creator.dir/Creator.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos/Creator.c > CMakeFiles/Creator.dir/Creator.c.i

CMakeFiles/Creator.dir/Creator.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/Creator.dir/Creator.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos/Creator.c -o CMakeFiles/Creator.dir/Creator.c.s

# Object files for target Creator
Creator_OBJECTS = \
"CMakeFiles/Creator.dir/Creator.c.o"

# External object files for target Creator
Creator_EXTERNAL_OBJECTS =

Creator: CMakeFiles/Creator.dir/Creator.c.o
Creator: CMakeFiles/Creator.dir/build.make
Creator: Libs/libFort/libfort-0.4.2/lib/libfort.a
Creator: CMakeFiles/Creator.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable Creator"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Creator.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Creator.dir/build: Creator
.PHONY : CMakeFiles/Creator.dir/build

CMakeFiles/Creator.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Creator.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Creator.dir/clean

CMakeFiles/Creator.dir/depend:
	cd /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos /home/abner/Desktop/Operativos/Proyecto1/ProyectoIOperativos/CMakeFiles/Creator.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/Creator.dir/depend
