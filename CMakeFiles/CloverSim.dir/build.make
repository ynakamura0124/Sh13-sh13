# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ynakamura/cloversim

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ynakamura/cloversim

# Include any dependencies generated for this target.
include CMakeFiles/CloverSim.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CloverSim.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CloverSim.dir/flags.make

CMakeFiles/CloverSim.dir/CloverSim.cpp.o: CMakeFiles/CloverSim.dir/flags.make
CMakeFiles/CloverSim.dir/CloverSim.cpp.o: CloverSim.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ynakamura/cloversim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/CloverSim.dir/CloverSim.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CloverSim.dir/CloverSim.cpp.o -c /home/ynakamura/cloversim/CloverSim.cpp

CMakeFiles/CloverSim.dir/CloverSim.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CloverSim.dir/CloverSim.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ynakamura/cloversim/CloverSim.cpp > CMakeFiles/CloverSim.dir/CloverSim.cpp.i

CMakeFiles/CloverSim.dir/CloverSim.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CloverSim.dir/CloverSim.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ynakamura/cloversim/CloverSim.cpp -o CMakeFiles/CloverSim.dir/CloverSim.cpp.s

# Object files for target CloverSim
CloverSim_OBJECTS = \
"CMakeFiles/CloverSim.dir/CloverSim.cpp.o"

# External object files for target CloverSim
CloverSim_EXTERNAL_OBJECTS =

CloverSim: CMakeFiles/CloverSim.dir/CloverSim.cpp.o
CloverSim: CMakeFiles/CloverSim.dir/build.make
CloverSim: source/libCloverSimLib.so
CloverSim: PhysicsList/source/libPhysicsListLib.so
CloverSim: LENSLong/source/libLENSLongLib.so
CloverSim: /usr/lib/x86_64-linux-gnu/libyaml-cpp.so.0.6.2
CloverSim: /home/ynakamura/opt/root/lib/libCore.so
CloverSim: /home/ynakamura/opt/root/lib/libImt.so
CloverSim: /home/ynakamura/opt/root/lib/libRIO.so
CloverSim: /home/ynakamura/opt/root/lib/libNet.so
CloverSim: /home/ynakamura/opt/root/lib/libHist.so
CloverSim: /home/ynakamura/opt/root/lib/libGraf.so
CloverSim: /home/ynakamura/opt/root/lib/libGraf3d.so
CloverSim: /home/ynakamura/opt/root/lib/libGpad.so
CloverSim: /home/ynakamura/opt/root/lib/libROOTDataFrame.so
CloverSim: /home/ynakamura/opt/root/lib/libTree.so
CloverSim: /home/ynakamura/opt/root/lib/libTreePlayer.so
CloverSim: /home/ynakamura/opt/root/lib/libRint.so
CloverSim: /home/ynakamura/opt/root/lib/libPostscript.so
CloverSim: /home/ynakamura/opt/root/lib/libMatrix.so
CloverSim: /home/ynakamura/opt/root/lib/libPhysics.so
CloverSim: /home/ynakamura/opt/root/lib/libMathCore.so
CloverSim: /home/ynakamura/opt/root/lib/libThread.so
CloverSim: /home/ynakamura/opt/root/lib/libMultiProc.so
CloverSim: /home/ynakamura/opt/root/lib/libROOTVecOps.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4Tree.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4FR.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4GMocren.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4visHepRep.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4RayTracer.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4VRML.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4OpenGL.so
CloverSim: /usr/lib/x86_64-linux-gnu/libXmu.so
CloverSim: /usr/lib/x86_64-linux-gnu/libXext.so
CloverSim: /usr/lib/x86_64-linux-gnu/libXt.so
CloverSim: /usr/lib/x86_64-linux-gnu/libICE.so
CloverSim: /usr/lib/x86_64-linux-gnu/libSM.so
CloverSim: /usr/lib/x86_64-linux-gnu/libX11.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4gl2ps.so
CloverSim: /usr/lib/x86_64-linux-gnu/libGL.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4vis_management.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4modeling.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4interfaces.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4persistency.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4error_propagation.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4readout.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4physicslists.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4tasking.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4run.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4event.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4tracking.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4parmodels.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4processes.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4analysis.so
CloverSim: /usr/lib/x86_64-linux-gnu/libexpat.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4digits_hits.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4track.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4particles.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4geometry.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4materials.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4graphics_reps.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4intercoms.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4global.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4clhep.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4zlib.so
CloverSim: /home/ynakamura/opt/geant4/lib/libG4ptl.so.0.0.2
CloverSim: CMakeFiles/CloverSim.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ynakamura/cloversim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable CloverSim"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CloverSim.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CloverSim.dir/build: CloverSim

.PHONY : CMakeFiles/CloverSim.dir/build

CMakeFiles/CloverSim.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CloverSim.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CloverSim.dir/clean

CMakeFiles/CloverSim.dir/depend:
	cd /home/ynakamura/cloversim && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ynakamura/cloversim /home/ynakamura/cloversim /home/ynakamura/cloversim /home/ynakamura/cloversim /home/ynakamura/cloversim/CMakeFiles/CloverSim.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CloverSim.dir/depend
