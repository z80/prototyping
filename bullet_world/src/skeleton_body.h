
#ifndef __SKELETON_BODY_H_
#define __SKELETON_BODY_H_

#include "irrlicht.h"
#include "irrBullet.h"
#include <memory>
#include "bullet_body.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;
using namespace io;

class XSkeletonPoser;
class btCompoundShape;

class SkeletonBody
{
public:
    SkeletonBody( IAnimatedMeshSceneNode * p_node,
                  float bodyMass = 0.5f, float jointMass=1.0f, float radius = 0.2f );
    ~SkeletonBody();

    void addToWorld( irrBulletWorld * world );
    void updateShape();

    static void cleanup( SkeletonBody * body, irrBulletWorld * world );

public:
    btVector3 m_rootShift;
    IAnimatedMeshSceneNode * node;
    btRigidBody     * m_rootBody;
    std::vector<btRigidBody *> m_bodies;
    std::vector<btFixedConstraint *> m_constraints;
};



#endif



