
#ifndef __ENTITY_WORLD_H_
#define __ENTITY_WORLD_H_

#include "Ogre.h"
#include "OgreFrameListener.h"

#include "btBulletDynamicsCommon.h"
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"

#include "entity.h"
#include "entity_part.h"
#include "entity_planet.h"

namespace Osp
{

/** This class contains dynamics simulation related objects.
 *
 *
 *
 */

class EntityWorld: public Entity, public Ogre::Singleton<EntityWorld>
{
public:
    static EntityWorld * createWorld();
    static void deleteWorld();

    EntityWorld( Ogre::SceneManager * scnMgr );
    ~EntityWorld();

    void integrationStep( Ogre::Real & t_sec, int timeBoost=1 );

    bool frameStarted( const Ogre::FrameEvent & evt, bool debugDraw = true );
    bool frameStarted( Ogre::Real dt, bool debugDraw = true );
    void addEntity( Block * part );
    void addEntity( EntityPlanet * part );
    void removeEntity( Block * part );
    void removeEntity( EntityPlanet * part );

public:
    btAxisSweep3                        * mBroadphase;
    btDefaultCollisionConfiguration     * mCollisionConfig;
    btCollisionDispatcher               * mDispatcher;
    btSequentialImpulseConstraintSolver * mSolver;
    btDynamicsWorld                     * phyWorld;
    BtOgre::DebugDrawer                 * dbgdraw;
};


}


#endif




