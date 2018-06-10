
set( proj "simple_ogre" )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set( OGRE_LIBRARIES ${proj} )
include( "${CMAKE_SOURCE_DIR}/settings.cmake" )
set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )
find_package( OGRE 1.11 REQUIRED )

message( WARNING ${OGRE_LIBRARIES} )
message( WARNING ${OGRE_INCLUDE_DIRS} )
message( WARNING ${OGRE_BITES_LIBRARIES} )



set( IBASE ${OGRE_HOME}/include/OGRE )
set( LBASE ${OGRE_HOME}/lib )

set( libs ${LBASE}/libOgreMain.so 
          ${LBASE}/libOgreBites.so 
          ${LBASE}/libOgreOverlay.so 
          ${LBASE}/libOgrePaging.so 
          ${LBASE}/libOgreProperty.so 
          ${LBASE}/libOgreRTShaderSystem.so 
          ${LBASE}/libOgreTerrain.so 
          ${LBASE}/libOgreVolume.so )


include_directories( "./src"
  ${OGRE_INCLUDE_DIRS}
  ${IBASE}/Bites 
  ${IBASE}/RTShaderSystem 
  ${IBASE} )


file( GLOB src "./src/*.cpp" )
add_executable( ${proj} ${src} )

target_link_libraries( ${proj} ${OGRE_LIBRARIES} ${libs} )
# copy resource.cfg next to our binaries where OGRE looks for it
#file(COPY ${OGRE_CONFIG_DIR}/resources.cfg DESTINATION ${CMAKE_BINARY_DIR})


