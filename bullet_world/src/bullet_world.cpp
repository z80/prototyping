
#include "bullet_world.h"
#include "bullet_body.h"

BulletWorld::BulletWorld( IrrlichtDevice * device )
{
    bool debugDraw = true;
    bool useGImpact = true;

    irrBulletWorld * w = createIrrBulletWorld( device, useGImpact, debugDraw );
    world.reset( w );

    /*
    ISceneNode * node = device->getSceneManager()->addCubeSceneNode( 1.0 );
    node->setScale(vector3df( 3000, 2, 3000) ); // 400, 3, 400
    node->setPosition( vector3df( 0.0f,-1.0f ,0.0f ) );
    node->setMaterialFlag(video::EMF_LIGHTING, true);
    node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
    node->setMaterialTexture(0, device->getVideoDriver()->getTexture("ground.png"));

    ICollisionShape * shape = new IBoxShape( node, 0, false );

    m_ground = world->addRigidBody( shape );
    */
}

BulletWorld::~BulletWorld()
{
    // Clean up all bodies.
    //world->addToDeletionQueue( m_ground );
}

void BulletWorld::setGravity( const core::vector3df & g )
{
    world->setGravity( g );
}

void BulletWorld::integratonStep( float dt, int maxSubsteps )
{
    world->stepSimulation( dt, maxSubsteps );
}

void BulletWorld::addBody( ISceneNode * node )
{
    BulletBody * body = new BulletBody( node );
    BulletBody * prev = this->body( node );
    if ( prev )
        // Or somehow in different way.
        delete prev;
    bodies[node] = body;
}

void BulletWorld::setShape( ISceneNode * node, const std::string & stri, float mass, IMesh * extra )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;
    body->setShape( stri, mass, extra );
    body->body = world->addRigidBody( body->bodyShape );
}

void BulletWorld::setAttribute( ISceneNode * node, const std::string & stri, bool value )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->setAttribute( stri, value );
}

void BulletWorld::applyForce( ISceneNode * node, const core::vector3df & force, const core::vector3df & at, bool local )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->applyForce( force, at, local );
}

void BulletWorld::applyTorque( ISceneNode * node, const core::vector3df & torque, bool local )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->applyTorque( torque, local );
}

void BulletWorld::setSpeed( ISceneNode * node, const core::vector3df & v, bool local )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->setSpeed( v, local );
}

void BulletWorld::setAngVel( ISceneNode * node, const core::vector3df & w, bool local )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->setAngVel( w, local );
}

void BulletWorld::setFriction( ISceneNode * node, float k )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->setFriction( k );
}

void BulletWorld::setLinearFactor( ISceneNode * node, float x, float y, float z )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->setLinearFactor( x, y, z );
}

void BulletWorld::setAngularFactor( ISceneNode * node, float x, float y, float z )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->setAngularFactor( x, y, z );
}

void BulletWorld::position( ISceneNode * node, core::vector3df & at )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->position( at );

}

void BulletWorld::orientation( ISceneNode * node, core::quaternion & q )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->orientation( q );
}

void BulletWorld::speed( ISceneNode * node, core::vector3df & v )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->speed( v );
}

void BulletWorld::angVel( ISceneNode * node, core::vector3df & w )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    body->angVel( w );
}

void BulletWorld::removeBody( ISceneNode * node )
{
    BulletBody * body = this->body( node );
    if ( !body )
        return;

    world->addToDeletionQueue( body->body );
    // And erase body from bodies list.
    std::map< ISceneNode *, BulletBody * >::iterator it = bodies.find( node );
    bodies.erase( it );
    // Delete body itself.
    delete body;
}

void BulletWorld::collideSkeleton( IAnimatedMeshSceneNode * node, float f, const std::string & attrib )
{
    ISkinnedMesh * mesh = dynamic_cast<ISkinnedMesh *>( node->getMesh() );
    if ( !mesh )
        return;

    core::matrix4 toWorld = node->getAbsoluteTransformation();

    core::vector3df from, to;
    btVector3       btFrom, btTo;

    u32 sz = mesh->getAllJoints().size();

    for ( u32 g=0; g<sz; g++ )
    {
        ISkinnedMesh::SJoint * joint = mesh->getAllJoints()[g];

        core::vector3df from = joint->GlobalAnimatedMatrix.getTranslation();
        // Convert to global RF.
        toWorld.transformVect( from );
        btFrom.setX( from.X );
        btFrom.setY( from.Y );
        btFrom.setZ( from.Z );

        u32 childrenSz = joint->Children.size();
        for ( u32 n=0; n<childrenSz; ++n )
        {
            to = joint->Children[n]->GlobalAnimatedMatrix.getTranslation();
            // Convert to global RF.
            toWorld.transformVect( to );
            btTo.setX( to.X );
            btTo.setY( to.Y );
            btTo.setZ( to.Z );

            btCollisionWorld::ClosestRayResultCallback cb( btFrom, btTo );

            world->getPointer()->rayTest( btFrom, btTo, cb );

            if ( cb.hasHit() )
            {
                const btCollisionObject * co = cb.m_collisionObject;
                if ( co->isStaticObject() )
                    continue;
                //co->getUserPointer();
                // Here has to check all one by one.
                BulletBody * body = this->body( co );
                if ( ( body ) && (
                         ( attrib.size() < 1 ) ||
                         ( body->body->getAttributes()->getAttributeAsBool( attrib.c_str() ) )
                                 )
                   )
                {
                    core::vector3df force( cb.m_hitNormalWorld.x(), cb.m_hitNormalWorld.y(), cb.m_hitNormalWorld.z() );
                    force = force * (-f);
                    core::vector3df at( cb.m_hitPointWorld.x(), cb.m_hitPointWorld.y(), cb.m_hitPointWorld.z() );
                    body->applyForce( force, at, false );
                }
            }
        }
    }
}

void BulletWorld::addSkeleton( IAnimatedMeshSceneNode * node, float jointMass, float radius )
{
    SkeletonBody * sb = new SkeletonBody( node, jointMass, jointMass, radius );
    sb->addToWorld( world.get() );
    skeletons[ node ] = sb;
}

void BulletWorld::processSkeletons()
{
    for ( std::map< IAnimatedMeshSceneNode *, SkeletonBody * >::iterator it=skeletons.begin();
          it!=skeletons.end(); it++ )
    {
        SkeletonBody * sb = it->second;
        sb->updateShape();
    }
}



BulletBody * BulletWorld::body( ISceneNode * node )
{
    std::map< ISceneNode *, BulletBody * >::iterator it = bodies.find( node );
    if ( it == bodies.end() )
        return 0;
    return it->second;
}

BulletBody * BulletWorld::body( const btCollisionObject * co )
{
    std::map< ISceneNode *, BulletBody * >::iterator it = bodies.begin();
    for ( ; it!=bodies.end(); it++ )
    {
        BulletBody * bb = it->second;
        if ( bb->body->getPointer() == co )
            return bb;
    }
    return 0;
}

void BulletWorld::cleanupSkeletons()
{
    for ( std::map< IAnimatedMeshSceneNode *, SkeletonBody * >::iterator it=skeletons.begin();
          it!=skeletons.end(); it++ )
    {
        SkeletonBody * sb = it->second;
        SkeletonBody::cleanup( sb, world.get() );
    }
    skeletons.clear();
}








