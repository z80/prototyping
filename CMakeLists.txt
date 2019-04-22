
cmake_minimum_required(VERSION 3.1)
project( "osp" )

set( CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/_app" )
set( URHO3D_HOME "${CMAKE_SOURCE_DIR}/_u_install/debug" )

add_subdirectory( "./u3d_template" )
add_subdirectory( "./tests" )



