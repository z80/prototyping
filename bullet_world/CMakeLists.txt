
set( proj "bullet_world" )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ROOT}/cmake )
SET( CMAKE_INCLUDE_CURRENT_DIR ON )


find_package(Boost COMPONENTS system filesystem regex thread REQUIRED)

INCLUDE_DIRECTORIES( ${ROOT}/3dparty/irrlicht/src
                     ${ROOT}/3dparty/irrlicht/include
                     ${ROOT}/3dparty/bullet-2.86.1/src
                     ${ROOT}/3dparty/irrbullet/src
                     ${ROOT}/3dparty/irrbullet/include
                     ${ROOT}/3dparty/lua-5.1.5/src
                     ${ROOT}/3dparty/lua-5.1.5/etc
                     ${ROOT}/nodes/src
                     ${ROOT}/irrlua/src
                     ./src )


file( GLOB src "./src/*.cpp" )

add_library( ${proj} STATIC ${src} )

set( libs ${Boost_LIBRARIES}
irrbullet
BulletSoftBody
BulletDynamics
BulletCollision
LinearMath
ConvexDecomposition
irrlicht
 )
target_link_libraries( ${proj} ${libs} )

install( TARGETS ${proj} DESTINATION "./" )

target_compile_features( ${proj} PRIVATE cxx_range_for )


