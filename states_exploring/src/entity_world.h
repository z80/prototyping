
#ifndef __ENTITY_WORLD_H_
#define __ENTITY_WORLD_H_

#include "Ogre.h"

#include "btBulletDynamicsCommon.h"
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"


/** This class contains dynamics simulation related objects.
 *
 *
 *
 */

class EntityWorld: Ogre::Singleton<EntityWorld>
{
public:
    EntityWorld();
    ~EntityWorld();

public:
    btAxisSweep3                        * mBroadphase;
    btDefaultCollisionConfiguration     * mCollisionConfig;
    btCollisionDispatcher               * mDispatcher;
    btSequentialImpulseConstraintSolver * mSolver;
    btDynamicsWorld                     * phyWorld;
    BtOgre::DebugDrawer                 * dbgdraw;
};




#endif




