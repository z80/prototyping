
#ifndef __ASSEMBLY_H_
#define __ASSEMBLY_H_

#include "entity_part.h"
#include "entity_planet.h"
#include "design.h"

namespace Entity
{

class AssembliesManager;

class Assembly
{
public:
    Assembly();
    ~Assembly();

    static Assembly * create( Design & design );

    void setR( const Ogre::Vector3 & r );
    void setQ( const Ogre::Quaternion & q );
    void setV( const Ogre::Vector3 & v );
    void setW( const Ogre::Vector3 & w );

    Ogre::Vector3    locR() const;
    Ogre::Quaternion locQ() const;
    Ogre::Vector3    locV() const;
    Ogre::Vector3    locW() const;

    Ogre::Vector3 absoluteR()    const;
    Ogre::Quaternion absoluteQ() const;
    Ogre::Vector3 absoluteV()    const;
    Ogre::Vector3 absoluteW()    const;

    void integrateDynamics( Ogre::Real t_sec, int timeBoost=1 );
    void setParent( EntityPlanet * planet );
    void setParentRf( EntityPlanet * planet );
    void partsToWorldRf();
    void partsToAssemblyRg();

    void partDestroyed( EntityPart * part );
    void connectionRemoved( EntityPart * partA, EntityPart * partB );
    void connectionEstablished( EntityPart * partA, EntityPart * partB );

    void computeAssemblyRQVW();
    void computePartsRQVW();

    Ogre::Vector3     r;
    Ogre::Quaternion  q;
    Ogre::Vector3     v;
    Ogre::Vector3     w;
    // Here not going to use rigid body so far.
    // May be later...


    bool              nearSurface;
    EntityPlanet    * parent;
    Ogre::SceneNode * sceneNode;

    std::vector<EntityPart *> parts;
    AssembliesManager * assembliesMgr;
};




}

#endif


