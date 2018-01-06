cmake_minimum_required( VERSION 2.8 )

project( MainApplication )
find_package( Threads )

set( CMAKE_BUILD_TYPE Release )
#set( CMAKE_BUILD_TYPE Debug )

#Bring the headers
include_directories( ${CMAKE_SOURCE_DIR}/Headers )
include_directories( ${CMAKE_SOURCE_DIR}/Headers/IO )
include_directories( ${CMAKE_SOURCE_DIR}/Headers/ControlSoftware )

file( GLOB_RECURSE Headers "Headers/*.h" )
file( GLOB_RECURSE Sources "Sources/*.c" "Sources/*.cpp" )
file( GLOB_RECURSE Sources "*.cpp" )

add_executable( ${PROJECT_NAME} ${Headers} ${Sources} )

target_link_libraries( ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} )
