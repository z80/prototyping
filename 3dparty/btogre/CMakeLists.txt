cmake_minimum_required(VERSION 2.8.6)

#project(BtOgre)

#find_package(Bullet REQUIRED)

include( "${CMAKE_SOURCE_DIR}/settings.cmake" )
set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )
find_package(OGRE 1.11 REQUIRED)


set( LBASE ${OGRE_HOME}/lib )
set( libs ${LBASE}/libOgreMain.so 
          ${LBASE}/libOgreBites.so 
          ${LBASE}/libOgreOverlay.so 
          ${LBASE}/libOgrePaging.so 
          ${LBASE}/libOgreProperty.so 
          ${LBASE}/libOgreRTShaderSystem.so 
          ${LBASE}/libOgreTerrain.so 
          )

# the search paths
include_directories( ${OGRE_INCLUDE_DIRS} 
                     ${OGRE_INCLUDE_DIR}/Bites 
                     ${OGRE_INCLUDE_DIR}/RTShaderSystem )

option(BUILD_DEMO "build demo application" TRUE)

include_directories(
    #${PROJECT_SOURCE_DIR}/include/
    #${BULLET_INCLUDE_DIRS}
    #${OGRE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${ROOT}/3dparty/bullet-2.87/src 
    ${OGRE_INCLUDES}
)
#link_directories(${OGRE_LIBRARY_DIRS})

add_definitions( -DBT_USE_DOUBLE_PRECISION )
add_library(BtOgre STATIC BtOgre.cpp)
target_link_libraries(BtOgre ${libs} BulletDynamics BulletCollision
                                          LinearMath )
target_compile_features( BtOgre PRIVATE cxx_range_for )

#INSTALL(TARGETS BtOgre
#  RUNTIME DESTINATION bin
#  LIBRARY DESTINATION lib
#  ARCHIVE DESTINATION lib)

if(BUILD_DEMO)
    add_executable(BtOgreTest demo/main.cpp)
    target_link_libraries(BtOgreTest BtOgre BulletDynamics BulletCollision
                                            LinearMath )
    target_compile_features( BtOgreTest PRIVATE cxx_range_for )

    #configure_file(demo/data/resources.cfg.in ${CMAKE_BINARY_DIR}/resources.cfg @ONLY)

    # copy essential config files next to our binary where OGRE autodiscovers them
    #file(COPY ${OGRE_CONFIG_DIR}/plugins.cfg DESTINATION ${CMAKE_BINARY_DIR})
endif()
