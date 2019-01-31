
#include "assemblies_manager.h"
#include "state_manager.h"
#include "entity_part.h"
#include "entity_planet.h"
#include "entity_world.h"
#include "assembly.h"
#include "part_manager_base.h"

#include "btBulletDynamicsCommon.h"


namespace Osp
{

AssembliesManager::AssembliesManager()
{
    planet = 0;
}

AssembliesManager::~AssembliesManager()
{

}

bool AssembliesManager::load( Config::ConfigReader * cr )
{

}

void AssembliesManager::setOrigin( EntityPlanet * planet,
                                   const Ogre::Vector3 & at,
                                   const Ogre::Quaternion & q )
{

}

Assembly * AssembliesManager::create( const Design & design )
{
    const bool designValid = design.valid();
    if ( !designValid )
        return 0;

    Assembly * a = new Assembly();
    a->assembliesMgr = this;
    //Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    a->sceneNode = planet->sceneNode->createChildSceneNode();
    Ogre::SceneNode * sn = a->sceneNode;
    sn->setInheritScale( false );
    sn->setInheritOrientation( true );
    sn->setPosition( at );
    sn->setOrientation( q );

    a->parent      = planet;
    a->nearSurface = true;


    // Add parts.
    StateManager * sm = StateManager::getSingletonPtr();
    PartManagerBase * pm = sm->getPartsManager();

    const size_t blocksQty = design.blocks.size();
    a->blocks.reserve( blocksQty );
    for ( size_t i=0; i<blocksQty; i++ )
    {
        const Design::Block & b = design.blocks[i];
        Block * block = pm->create( b.name );
        block->setR( b.r );
        block->setQ( b.q );
        block->assembly    = a;
        block->assemblyInd = i;
        a->blocks.push_back( block );
    }

    // Add connections.
    const size_t jointsQty = design.joints.size();
    a->connections.reserve( jointsQty );
    for ( size_t i=0; i<jointsQty; i++ )
    {
        const Design::Joint & c = design.joints[i];
        BlockConnection * joint = new BlockConnection();
        joint->blockA = a->blocks[c.blockA];
        joint->blockB = a->blocks[c.blockB];
        Ogre::Vector3    rel_r;
        Ogre::Quaternion rel_q;
        joint->blockB->relativePose( joint->blockA, rel_r, rel_q );
        joint->blockB->setSceneParent( joint->blockA );
        btTransform trA;
        trA.setOrigin( btVector3( rel_r.x, rel_r.y, rel_r.z ) );
        trA.setRotation( btQuaternion( rel_q.w, rel_q.x, rel_q.y, rel_q.z ) );
        btTransform trB;
        trB.setIdentity();
        joint->constraint = new btFixedConstraint( *joint->blockA->rigidBody,
                                                   *joint->blockB->rigidBody,
                                                   trA, trB );
        joint->assembly    = a;
        joint->assemblyInd = i;

        a->connections.push_back( joint );
    }

    // Compute center of inertia.
    a->computeCenterOfInertia();
    a->computeAssemblyRQVW();

    assemblies.push_back( a );

    return a;
}

Assembly * AssembliesManager::create( const Ogre::String & fname )
{
    Design design;
    const bool designLoadedOk = design.load( fname );
    if ( !designLoadedOk )
        return 0;

    Assembly * a = create( design );
    return a;
}

Assembly * AssembliesManager::create( const std::vector<Block *> & parts,
                                      const std::vector<BlockConnection *> & connections )
{
    Assembly * a = new Assembly();
    a->assembliesMgr = this;
    //Ogre::SceneManager * smgr = StateManager::getSingletonPtr()->getSceneManager();
    a->sceneNode = planet->sceneNode->createChildSceneNode();
    Ogre::SceneNode * sn = a->sceneNode;
    sn->setInheritScale( false );
    sn->setInheritOrientation( true );
    sn->setPosition( at );
    sn->setOrientation( q );

    a->parent      = planet;
    a->nearSurface = true;


    // Add parts.
    a->blocks       = parts;
    a->connections = connections;
    // Set each part assembly to be this one.
    const size_t partQty = parts.size();
    for ( size_t i=0; i<partQty; i++ )
    {
        Block * p = parts[i];
        p->assembly = a;
    }

    // Compute center of inertia.
    a->computeCenterOfInertia();
    a->computeAssemblyRQVW();

    assemblies.push_back( a );

    return a;
}

void AssembliesManager::destroy( Assembly * assembly )
{
    const size_t qty = assemblies.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Assembly * a = assemblies[i];
        if ( a == assembly )
        {
            if ( i<(qty-1) )
                assemblies[i] = assemblies[qty-1];
            assemblies.resize( qty-1 );
        }
    }
}

int AssembliesManager::designsQty() const
{
    const int qty = static_cast<int>( designs.size() );
}

Design * AssembliesManager::design( int ind )
{
    const int qty = static_cast<int>( designs.size() );
    Design * d = ( ind < qty ) ? &(designs[ind]) : 0;
    return d;
}

void AssembliesManager::integrateDynamics( Ogre::Real time_s, int time_boost )
{
    bool needIntegration = false;
    const size_t qty = assemblies.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Assembly * a = assemblies[i];
        a->integrateDynamics( time_s, time_boost );
        if ( a->forcesApplied() )
            needIntegration = true;
    }
    // All assemblies experiencing forces other than gravity
    // are moved here.
    // For those poses and velocities are supposed to be recalculated.
    if ( needIntegration )
    {
        DynamicsWorld * w = StateManager::getSingletonPtr()->getWorld();
        w->integrationStep( time_s, time_boost );
        for ( size_t i=0; i<qty; i++ )
        {
            Assembly * a = assemblies[i];
            if ( a->forcesApplied() )
                a->computeAssemblyRQVW();
        }
    }
}

void AssembliesManager::cleanup()
{
    const size_t qty = assemblies.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Assembly * a = assemblies[i];
        delete a;
    }
    // The array is supposed to be empty at this point as
    // on deletion assemblies are supposed to remove themself
    // from the array.
}


}


