
#ifndef __BULLET_BODY_H_
#define __BULLET_BODY_H_

#include <irrlicht.h>
#include <irrBullet.h>
#include <memory>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;
using namespace io;


class BulletBody
{
public:
    BulletBody( ISceneNode * p_node );
    ~BulletBody();

    bool operator==( const btCollisionShape * shape ) const;

    void setShape( const std::string & shape, float mass, IMesh * extra = 0 );
    void setAttribute( const std::string & name, bool value );

    void applyForce( const core::vector3df & force, const core::vector3df & at, bool rel = false );
    void applyTorque( const core::vector3df & torque, bool rel = false );
    void setSpeed( const core::vector3df & v, bool res = false );
    void setAngVel( const core::vector3df & w, bool res = false );
    void setFriction( float k );

    void setLinearFactor( float x, float y, float z );
    void setAngularFactor( float x, float y, float z );

    void position( core::vector3df & at );
    void orientation( core::quaternion & q );
    void speed( core::vector3df & v );
    void angVel( core::vector3df & w );

public:
    ISceneNode      * node;
    IRigidBody      * body;
    ICollisionShape * bodyShape;

};


#endif


