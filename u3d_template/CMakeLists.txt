
set( proj_name "osp" )

# Set project name
#project( ${proj_name} )

add_subdirectory( "./3dparty" )

# Set minimum version
cmake_minimum_required (VERSION 2.8.6)
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    if (CMAKE_VERSION VERSION_GREATER 2.8.12 OR CMAKE_VERSION VERSION_EQUAL 2.8.12)
        # INTERFACE_LINK_LIBRARIES defines the link interface
        cmake_policy (SET CMP0022 NEW)
    endif ()
    if (CMAKE_VERSION VERSION_GREATER 3.0.0 OR CMAKE_VERSION VERSION_EQUAL 3.0.0)
        # Disallow use of the LOCATION target property - therefore we set to OLD as we still need it
        cmake_policy (SET CMP0026 OLD)
        # MACOSX_RPATH is enabled by default
        cmake_policy (SET CMP0042 NEW)
    endif ()
endif ()

macro( header_directories root_path return_list )
    file( GLOB_RECURSE new_list ${root_path}/*.h )
    set( dir_list "" )
    foreach( file_path ${new_list} )
        get_filename_component( dir_path ${file_path} PATH )
        set(dir_list ${dir_list} ${dir_path})
    endforeach()
    list( REMOVE_DUPLICATES dir_list )
    set( ${return_list} ${dir_list} )
endmacro()

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules)

# Include Urho3D Cmake common module
include (UrhoCommon)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

file( GLOB_RECURSE SRCS src/*.cpp )
file( GLOB_RECURSE HDRS include/*.h )

#add_definitions( -DURHO3D_DATABASE_SQLITE )
add_definitions( -DURHO3D_ANGELSCRIPT )
add_definitions( -DURHO3D_IK )
add_definitions( -DURHO3D_LUA )
add_definitions( -DURHO3D_NAVIGATION )
add_definitions( -DURHO3D_NETWORK )
add_definitions( -DURHO3D_PHYSICS )
add_definitions( -DURHO3D_URHO2D )
add_definitions( -DBT_USE_DOUBLE_PRECISION )

# Find Urho3D library
find_package (Urho3D REQUIRED)

#header_directories( "./" include_dirs )

include_directories( ${URHO3D_INCLUDE_DIRS}
                     "./3dparty/tinyxml2"
                     "./3dparty/lemon-1.3.1"
                     "./3dparty/eigen-3.3.7"
                     "./Source"
                     "./Source/UI"
                     "./Source/src/data_types"
                     "./Source/src/cubesphere"
                     "./Source/src/dynamics"
                     "./Source/src/utils"
                     "./Source/src/block"
                     "./Source/src/blocks"
                     "./Source/src/planet"
                     "./Source/src/design"
                     "./Source/src/item_base"
                     "./Source/src/camera_ctrl"
                     "./Source/src/player"
                     "./Source/src/modes"
                     "./Source/src/levels"
                     "./Source/src/levels/workshop"
                     "./Source/src/levels/workshop/ui"
                     "./Source/src/levels/one_planet"
                     "./Source/src/levels/level_main"
                     "${CMAKE_CURRENT_BINARY_DIR}/3dparty/lemon-1.3.1" )
message( "dir is: ${CURRENT_BINARY_DIR}" )

# Define target name
set( TARGET_NAME ${proj_name} )

define_source_files(GLOB_CPP_PATTERNS Source/*.c* GLOB_H_PATTERNS Source/*.h* RECURSE GROUP )

#set(INCLUDE_DIRS include rapidjson/include)
set(INCLUDE_DIRS include)

# Setup target with resource copying
setup_main_executable ()
target_link_libraries( ${TARGET_NAME} tinyxml2 lemon )


ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)
