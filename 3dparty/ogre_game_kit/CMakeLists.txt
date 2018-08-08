
set( proj "oge" )

file( GLOB src "./common/*.cpp" )
file( GLOB s   "./common/lib/collision/*.cpp" )
set( src ${src} ${s} )
file( GLOB s   "./common/lib/gui/*.cpp" )
set( src ${src} ${s} )
file( GLOB s   "./common/lib/scene/*.cpp" )
set( src ${src} ${s} )
file( GLOB s   "./common/lib/xml/*.cpp" )
set( src ${src} ${s} )

include_directories( "./common" 
                     "./common/lib/collision" 
                     "./common/lib/gui" 
                     "./common/lib/scene" 
                     "./common/lib/xml" )


include( ${CMAKE_SOURCE_DIR}/settings.cmake )
set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

find_package( OGRE REQUIRED )

include_directories( ${OGRE_INCLUDE_DIRS} )
include_directories( ${OGRE_INCLUDE_DIR}/Bites
                     ${OGRE_INCLUDE_DIR}/Overlay
                     ${OGRE_INCLUDE_DIR}/RTShaderSystem )
link_directories( ${OGRE_LIBRARY_DIRS} )

add_library( ${proj} ${src} )
target_link_libraries( ${proj} ${OGRE_LIBRARIES} )
target_compile_features( ${proj} PRIVATE cxx_range_for )

