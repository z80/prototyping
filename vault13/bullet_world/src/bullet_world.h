
#ifndef __BULLET_WORLD_H_
#define __BULLET_WORLD_H_

#include <irrlicht.h>
#include <irrBullet.h>
#include <memory>

#include "skeleton_body.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;
using namespace io;

class BulletBody;


// Don't forget that it is also possible to add
// some sort of reaction on collisiont to play sound, for example.
// For that it might be enough to have method saying if there was a collision
// during the latest interation and what bodies participated.
// Or some sort of callback....

class BulletWorld
{
public:
    BulletWorld( IrrlichtDevice * device );
    ~BulletWorld();

    void setGravity( const core::vector3df & g );

    void integratonStep( float dt, int maxSubsteps );

    void addBody( ISceneNode * node );
    void setShape( ISceneNode * node, const std::string & stri, float mass, IMesh * extra = 0 );
    void setAttribute( ISceneNode * node, const std::string & stri, bool value );
    void applyForce( ISceneNode * node, const core::vector3df & force, const core::vector3df & at, bool local = false );
    void applyTorque( ISceneNode * node, const core::vector3df & torque, bool local = false );
    void setSpeed( ISceneNode * node, const core::vector3df & v, bool local = false );
    void setAngVel( ISceneNode * node, const core::vector3df & w, bool local = false );
    void setFriction( ISceneNode * node, float k );
    void setLinearFactor( ISceneNode * node, float x, float y, float z );
    void setAngularFactor( ISceneNode * node, float x, float y, float z );

    void position( ISceneNode * node, core::vector3df & at );
    void orientation( ISceneNode * node, core::quaternion & q );
    void speed( ISceneNode * node, core::vector3df & v );
    void angVel( ISceneNode * node, core::vector3df & w );


    void removeBody( ISceneNode * node );

    void collideSkeleton( IAnimatedMeshSceneNode * node, float f, const std::string & attrib );

    // Dynamical skeletons.
    void addSkeleton( IAnimatedMeshSceneNode * node, float jointMass=1.0f, float radius=0.2f );
    void processSkeletons();
public:
    BulletBody * body( ISceneNode * node );
    BulletBody * body( const btCollisionObject * co );

    void cleanupSkeletons();

    std::shared_ptr<irrBulletWorld> world;

    std::map< ISceneNode *, BulletBody * > bodies;

    std::map< IAnimatedMeshSceneNode *, SkeletonBody * > skeletons;

    // Invisible infinite plane which always presents.
    // Otherwise due to some reason dynamical skeleton goes deeply
    // through the surface of mesh surface.
    IRigidBody * m_ground;
};



#endif



