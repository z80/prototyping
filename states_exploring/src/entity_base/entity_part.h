
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

class EntityPlanet;

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

    bool setEntity( const char * mesh, const char * material = 0 );
    bool setMaterial( const char * material );
    bool setMask( Ogre::uint32 mask );
    bool setAirMesh( const std::string & meshFileName );
    void addConnectionPoint( const ConnectionPoint & pt );

    void setMass( Ogre::Real & m );
    void setInertia( const Ogre::Vector3 & i );
    void setCollisionShape( btCollisionShape * shape );
    void initDynamics();

    void applyForce( const Ogre::Vector3 & at, const Ogre::Vector3 & f );
    void applyTorque( const Ogre::Vector3 & p );

    void setR( const Ogre::Vector3 & at );
    Ogre::Vector3 relR() const;
    void setQ( const Ogre::Quaternion & q );
    Ogre::Quaternion relQ() const;
    Ogre::Vector3 relV() const;
    void          relV( Ogre::Real & v ) const;
    void setV( Ogre::Vector3 & v );
    Ogre::Vector3 relW() const;
    void setW( const Ogre::Vector3 & w );

    void setParent( EntityPlanet * planet );
    void setParentRf( EntityPlanet * planet );

    bool addSound( const std::string & fileName, const std::string & name );
    void finalizeSounds();
    bool playSound( const std::string & name, bool looped = false );
    bool stopSound( const std::string & name );

public:
    // Specific functions for changing a parent.
    Ogre::Vector3    absoluteV() const;
    Ogre::Vector3    absoluteR() const;
    Ogre::Vector3    absoluteW() const;
    Ogre::Quaternion absoluteQ() const;

    void deleteRigidBody();
    void deleteCollisionShape();
    void deleteVisual();

    Ogre::int32 actionGroup;
    bool        isSeleted;
    bool        doShowContextMenu;

    Ogre::Entity     * visualEntity;
    Ogre::SceneNode  * sceneNode;
    btRigidBody      * rigidBody;
    btCollisionShape * collisionShape;
    BtOgre::RigidBodyState * bodyState;

    AirMesh::AirMesh   airMesh;

    EntityPlanet * parent;
    /// It rotates with planet. If true, only
    /// relative to the surface velocity matters.
    /// If false, just moves with respect to it's
    /// parent planet.
    bool           nearSurface;

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

typedef std::shared_ptr<EntityPart> EntityPartPtr;



}

#endif




