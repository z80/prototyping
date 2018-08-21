
#ifndef __ENTITY_PART_H_
#define __ENTITY_PART_H_

#include "entity.h"
#include "ImguiManager.h"

#include "btBulletDynamicsCommon.h"
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"
#include "OgreOggSoundManager.h"
#include "name_generator.h"

#include "air_mesh.h"

class StateManager;

namespace Entity
{

struct Sound
{
    std::string name;
    OgreOggSound::OgreOggISound * s;

    static struct _Lt
    {
        bool operator()( const Sound & a, const Sound & b )
        {
            const bool res = (a.name < b.name);
            return res;
        }
        bool operator()( const Sound & a, const std::string & b )
        {
            const bool res = (a.name < b);
            return res;
        }
        bool operator()( const std::string & a, const Sound & b )
        {
            const bool res = (a < b.name);
            return res;
        }
    } Lt;
};

struct ConnectionPoint
{
    /// Connection point position in local ref. frame.
    Ogre::Vector3 at;
    /// Connection point "direction"
    /// Connected objects are to have opposite directions.
    Ogre::Vector3 dir;
    /// Connection name (Not sure if I need that though...)
    std::string   name;
};


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



    bool setEntity( const std::string & mesh, const std::string & material );
    bool setAirMesh( const std::string & meshFileName );
    void addConnectionPoint( const ConnectionPoint & pt );

    void setMass( Ogre::Real & m );
    void setIntertia( const Ogre::Vector3 & i );
    void setCollisionShape( btCollisionShape * shape );
    void initDynamics();

    void applyForce( const Ogre::Vector3 & at, const Ogre::Vector3 & f );
    void applyTorque( const Ogre::Vector3 & p );

    void setPosition( const Ogre::Vector3 & at );
    void setOrientation( const Ogre::Quaternion & q );

    bool addSound( const std::string & fileName, const std::string & name );
    void finalizeSounds();
    bool playSound( const std::string & name, bool looped = false );
    bool stopSound( const std::string & name );

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

    std::vector<ConnectionPoint>  connections;
    std::vector<Sound>            sounds;
    Ogre::Real    mass;
    Ogre::Vector3 inertia;

    std::string name;
    std::string desc;
    std::string group;
    std::string icon;
    int         level;

};




}

#endif




