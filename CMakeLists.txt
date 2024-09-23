cmake_minimum_required (VERSION 3.8)
#----------------------------------------------------------------------------
# Setup the project
project ("CloverSim")
set(CMAKE_CXX_STANDARD 17)

# find yaml-cpp package
find_package(yaml-cpp)
set(YAML_CPP_LIB_DIR ${YAML_CPP_CMAKE_DIR}/../../../lib CACHE PATH "yaml-cpp lib dir")

# find Geant4 package
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# find ROOT package.
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS Core RIO Net Hist Tree TreePlayer)
#include(${ROOT_USE_FILE})
include_directories($ENV{ROOTSYS}/include)

# Set the default install prefix to [source_dir]/install
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install CACHE PATH "install dir" FORCE)

option(CLOVERSIM_USE_BRIKEN "Use BRIKEN" OFF)
if (CLOVERSIM_USE_BRIKEN)
	set(BRIKEN_TOOLS_DIR ~/opt/ur17b/BrikenTools CACHE PATH "BrikenTools dir (required when CLOVERSIM_USE_BRIKEN is on)")
	set(BRIKEN_TOOLS_MERGER_DIR ${BRIKEN_TOOLS_DIR}/Merger/MergerSoft)
	include_directories(${BRIKEN_TOOLS_MERGER_DIR})
	link_directories(${BRIKEN_TOOLS_DIR}/Merger/build)
	add_definitions(-DUSE_BRIKEN)
endif(CLOVERSIM_USE_BRIKEN)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/PhysicsList/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/LENSLong/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/BRIKEN/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${YAML_CPP_INCLUDE_DIR})
link_directories(${YAML_CPP_LIB_DIR})
link_directories(${PWD}/PhysicsList/source)
link_directories(${PWD}/LENSLong/source)
link_directories(${PWD}/BRIKEN/source)
link_directories(${PWD}/source)
link_directories(${ROOTSYS}/lib)

add_subdirectory(PhysicsList)
add_subdirectory(LENSLong)
add_subdirectory(source)
if (CLOVERSIM_USE_BRIKEN)
	add_subdirectory(BRIKEN)
endif (CLOVERSIM_USE_BRIKEN)
add_executable (CloverSim "CloverSim.cpp")
#add_executable (main "test.cpp")

target_link_libraries(CloverSim
	CloverSimLib
	PhysicsListLib
	LENSLongLib 
)

#target_link_libraries(main
#	${ROOT_LIBRARIES}
#	#BRIKENLib
#	#CloverSimLib
#	#PhysicsListLib
#	#LENSLongLib
#	#data2TreeLib
#)

if (CLOVERSIM_USE_BRIKEN)
	target_link_libraries(CloverSim	BRIKENLib)
endif (CLOVERSIM_USE_BRIKEN)

install(TARGETS CloverSim
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include
)

#install(TARGETS main
#	RUNTIME DESTINATION bin
#	LIBRARY DESTINATION lib
#	PUBLIC_HEADER DESTINATION include)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/thiscloversim.sh.in"
    "${CMAKE_SOURCE_DIR}/thiscloversim.sh"
    @ONLY
)
