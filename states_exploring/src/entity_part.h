
#ifndef __ENTITY_PART_H_
#define __ENTITY_PART_H_

#include "entity.h"
#include "ImguiManager.h"

#include "btBulletDynamicsCommon.h"
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"

#include "air_mesh.h"

class StateManager;

namespace Entity
{


class EntityPart: public Entity
{
public:
    EntityPart();
    virtual ~EntityPart();

    virtual void loadResources();

    void setActionGroup( Ogre::int32 group );
    virtual void action( Ogre::int32 actionGroup, const Ogre::String & name, Ogre::Real value );

    virtual void drawGui();
    virtual void drawContextMenu();
    virtual void computeAirFriction();

    virtual void selectEvent();
    virtual void unselectEvent();
    virtual void contextMenuEvent();

    void speed( Ogre::Vector3 & v ) const;
    void speed( Ogre::Real & v ) const;

public:
    Ogre::int32 actionGroup;
    bool        isSeleted;
    bool        doShowContextMenu;

    Ogre::Entity     * visualEntity;
    Ogre::SceneNode  * sceneNode;
    btRigidBody      * rigidBody;
    btCollisionShape * collisionShape;
    BtOgre::RigidBodyState * bodyState;

    AirMesh::AirMesh   airMesh;
};




}

#endif




