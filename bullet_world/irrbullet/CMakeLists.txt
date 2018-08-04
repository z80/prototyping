
set( proj "irrbullet" )

include_directories( "./src" 
                     "./include" 
                     "${ROOT}/3dparty/bullet-2.86.1/src" 
                     "${ROOT}/3dparty/irrlicht/include"
                     "${ROOT}/3dparty/irrlicht/src" )

aux_source_directory( "./src" src )
add_library( ${proj} STATIC ${src} )
set_property( TARGET ${proj} PROPERTY COMPILE_DEFINITIONS ${defs} )
set_property( TARGET ${proj} PROPERTY OUTPUT_NAME ${proj} )
set_target_properties( ${proj} PROPERTIES PREFIX "" )

set( libs irrlicht
  ConvexDecomposition
  HACD
  GIMPACTUtils
  BulletInverseDynamicsUtils
  BulletFileLoader
  BulletXmlWorldImporter
  BulletWorldImporter
  Bullet3OpenCL_clew
  Bullet2FileLoader
  Bullet3Dynamics
  Bullet3Collision
  Bullet3Geometry
  Bullet3Common
  BulletInverseDynamics
  BulletSoftBody
  BulletCollision
  BulletDynamics
  LinearMath )

target_link_libraries( ${proj} ${libs} )

target_compile_features( ${proj} PRIVATE cxx_range_for )


