
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

namespace Osp
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

class EntityWorld;
class EntityPlanet;
class Block;
class Assembly;
class DynamicsWorld;


class BlockConnection
{
public:
    BlockConnection();
    ~BlockConnection();

    void toWorld( DynamicsWorld * w );
    void fromWorld();

    void destroy();

    Block * blockA;
    Block * blockB;
    btTypedConstraint * constraint;
    Assembly          * assembly;
    int                 assemblyInd;
    DynamicsWorld     * world;
};

class Block: public Entity
{
public:
    Block();
    virtual ~Block();

    virtual void loadResources();

    void setActionGroup( Ogre::int32 group );
    virtual void action( Ogre::int32 actionGroup, const Ogre::String & name, Ogre::Real value );

    virtual void drawGui();
    virtual void drawContextMenu();
    virtual void computeAirFriction();

    virtual void selectEvent();
    virtual void unselectEvent();
    virtual void contextMenuEvent();

    /// Virtual because in implementations
    /// there might be more than one rigid body.
    virtual void toWorld( DynamicsWorld * w );
    virtual void fromWorld();

    bool setEntity( const char * mesh, const char * material = 0 );
    bool setMaterial( const char * material );
    bool setMask( Ogre::uint32 mask );
    bool setAirMesh( const std::string & meshFileName );

    void setMass( Ogre::Real & m );
    void setInertia( const Ogre::Vector3 & i );
    void setCollisionShape( btCollisionShape * shape );
    void initDynamics();

    /// Returns true if during this step the part
    /// experiences force/torque applied.
    bool forcesApplied() const;
    /// Resets forcesApplied() flag.
    void resetForcesApplied();
    void applyForce( const Ogre::Vector3 & f, const Ogre::Vector3 & at, bool localRf=true );
    void applyTorque( const Ogre::Vector3 & p );

    void setR( const Ogre::Vector3 & at );
    Ogre::Vector3 relR() const;
    void setQ( const Ogre::Quaternion & q );
    Ogre::Quaternion relQ() const;
    Ogre::Vector3 relV() const;
    void          relV( Ogre::Real & v ) const;
    void setV( const Ogre::Vector3 & v );
    Ogre::Vector3 relW() const;
    void setW( const Ogre::Vector3 & w );

    // Are not supposed to be used.
    // Assembly manages all parenting stuff.
    //void setParent( EntityPlanet * planet );
    //void setParentRf( EntityPlanet * planet );

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

    bool forcesWereApplied;

    Ogre::int32 actionGroup;
    bool        isSeleted;
    bool        doShowContextMenu;

    Ogre::Entity     * visualEntity;
    /// Need a separate scene node as if any scale is envolved
    /// Ogre goes nuts with _getDerivedOrientation()
    Ogre::SceneNode  * visualNode;

    DynamicsWorld    * world;
    btRigidBody      * rigidBody;
    btCollisionShape * collisionShape;
    BtOgre::RigidBodyState * bodyState;

    AirMesh::AirMesh   airMesh;

    std::vector<Sound>            sounds;

    Ogre::Real    mass;
    Ogre::Vector3 inertia;

    std::string name;
    std::string desc;
    std::string group;
    std::string icon;
    int         level;

    /// Assembly this part is part of.
    Assembly       * assembly;
    /// Part position within the assembly.
    Ogre::Vector3    assemblyR;
    Ogre::Quaternion assemblyQ;
    /// Part index in assembly.
    int assemblyInd;
    /// Functionality to convert position and velocity to/from assembly.
    /// It is supposed to be used when true dynamics is turned on/off.
    void toAssembly();
    void fromAssembly();
};

typedef std::shared_ptr<Block> EntityPartPtr;



}

#endif




