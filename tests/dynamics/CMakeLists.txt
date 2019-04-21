
# Define target name
set( proj_name test_dynamics )

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/u3d_template/CMake/Modules)
include( UrhoCommon )
find_package( Urho3D REQUIRED )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY bin )

include_directories( "./src" ${URHO3D_INCLUDE_DIRS} )

file( GLOB SRCS "./src/*.cpp" )
file( GLOB HDRS "./src/*.h" )
#aux_source_directory( "./src" )

# Setup target with resource copying
#add_executable( ${proj} ${src} )

set( TARGET_NAME ${proj_name} )
define_source_files( GLOB_CPP_PATTERNS ./src/*.c* GLOB_H_PATTERNS ./src/*.h* RECURSE GROUP )

#set(INCLUDE_DIRS include rapidjson/include)
set( INCLUDE_DIRS ./src )

# Setup target with resource copying
setup_main_executable ()
target_link_libraries( ${TARGET_NAME} tinyxml2 lemon )


add_definitions( -DURHO3D_ANGELSCRIPT )
add_definitions( -DURHO3D_IK )
add_definitions( -DURHO3D_LUA )
add_definitions( -DURHO3D_NAVIGATION )
add_definitions( -DURHO3D_NETWORK )
add_definitions( -DURHO3D_PHYSICS )
add_definitions( -DURHO3D_URHO2D )
add_definitions( -DBT_USE_DOUBLE_PRECISION )

ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)



