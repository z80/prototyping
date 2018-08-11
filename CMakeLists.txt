
cmake_minimum_required(VERSION 3.1)
project( "my_procedural" )

set( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/_app )
set( ROOT ${CMAKE_CURRENT_SOURCE_DIR} )


set( IBASE ${OGRE_HOME}/include/OGRE )
set( LBASE ${OGRE_HOME}/lib )

set( OGRE_LIBS ${LBASE}/libOgreMain.so 
               ${LBASE}/libOgreBites.so 
               ${LBASE}/libOgreOverlay.so 
               ${LBASE}/libOgrePaging.so 
               ${LBASE}/libOgreProperty.so 
               ${LBASE}/libOgreRTShaderSystem.so 
               ${LBASE}/libOgreTerrain.so )

set( OGRE_INCLUDES
  ${OGRE_INCLUDE_DIRS}
  ${IBASE}
  ${IBASE}/Bites 
  ${IBASE}/RTShaderSystem 
  ${IBASE}/Overlay
   )

#set( BBASE ${ROOT}/3dparty/bullet-2.87/src )
set( BULLET_LIBS 
  #Bullet3Collision
  #Bullet3Common 
  #Bullet3Dynamics 
  #Bullet3Geometry 
  #Bullet3OpenCL 
  #Bullet3Serialize 
  BulletCollision 
  BulletDynamics 
  BulletInverseDynamics 
  BulletSoftBody 
  LinearMath
   )


add_subdirectory( "./3dparty" )
add_subdirectory( "./config_reader" )
add_subdirectory( "./simple" )
add_subdirectory( "./volume" )
add_subdirectory( "./volume_tree" )
add_subdirectory( "./icosphere" )
add_subdirectory( "./material_viewer" )
add_subdirectory( "./states_exploring" )



