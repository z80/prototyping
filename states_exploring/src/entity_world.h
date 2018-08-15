
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

namespace Entity
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

    bool frameStarted( const Ogre::FrameEvent & evt );
    void addEntity( EntityPart * part );
    void addEntity( EntityPlanet * part );

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




