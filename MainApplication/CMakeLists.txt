cmake_minimum_required( VERSION 2.8 )

# Name the project
project( MainApplication )

# Find all of the libraries needed to build project
find_package( Threads )
find_package( BlueZ )

# Set the active build type
set( CMAKE_BUILD_TYPE Release )
#set( CMAKE_BUILD_TYPE Debug )

# Add compiler flags
set( CMAKE_CXX_FLAGS "-Wall" )

# Bring in the Beagle Bone Blue library
include_directories( ${CMAKE_SOURCE_DIR}/BBBLibrary/Headers )
include_directories( ${CMAKE_SOURCE_DIR}/BBBLibrary/Headers/IO )
include_directories( ${CMAKE_SOURCE_DIR}/BBBLibrary/Headers/Bluetooth )
include_directories( ${CMAKE_SOURCE_DIR}/BBBLibrary/Headers/Math )
include_directories( ${CMAKE_SOURCE_DIR}/BBBLibrary/Headers/MotorControl )
include_directories( ${CMAKE_SOURCE_DIR}/BBBLibrary/Headers/PRU )

# Bring in the project headers
include_directories( ${CMAKE_SOURCE_DIR}/Headers )
include_directories( ${CMAKE_SOURCE_DIR}/Headers/Bluetooth )
include_directories( ${CMAKE_SOURCE_DIR}/Headers/ControlSoftware )
include_directories( ${CMAKE_SOURCE_DIR}/Headers/IO )

# Add all of the files to the build
file( GLOB_RECURSE Headers ${CMAKE_SOURCE_DIR}/Headers/*.h )
file( GLOB_RECURSE Headers ${CMAKE_SOURCE_DIR}/*.h )
file( GLOB_RECURSE Sources ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/Sources/*.cpp )

# Add the executable output
add_executable( ${PROJECT_NAME} ${Headers} ${Sources} )

# Setup all of the link libraries
target_link_libraries( ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries( ${PROJECT_NAME} ${BLUEZ_LIBRARIES} )
