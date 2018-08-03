# App_BasicExample is a minimal sample creating, stepping and deleting a Bullet dynamics world

INCLUDE_DIRECTORIES(
${BULLET_PHYSICS_SOURCE_DIR}/src
${BULLET_PHYSICS_SOURCE_DIR}/examples/BasicDemo
)

LINK_LIBRARIES(
 BulletDynamics BulletCollision LinearMath
)

	ADD_EXECUTABLE(App_BasicExample
		BasicExample.cpp
		main.cpp
	)





