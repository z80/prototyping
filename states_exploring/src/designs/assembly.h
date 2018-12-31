
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

    Ogre::Vector3    relR() const;
    Ogre::Quaternion relQ() const;
    Ogre::Vector3    relV() const;
    Ogre::Vector3    relW() const;

    Ogre::Vector3 absoluteR()    const;
    Ogre::Quaternion absoluteQ() const;
    Ogre::Vector3 absoluteV()    const;
    Ogre::Vector3 absoluteW()    const;

    void integrateDynamics( Ogre::Real t_sec, int timeBoost=1 );
    bool forcesApplied() const;

    void setParent( EntityPlanet * planet );
    void setParentRf( EntityPlanet * planet );

    void deletePart( EntityPart * part );
    void deleteConnection( EntityPart * partA, EntityPart * partB );
    void connectionEstablished( EntityPart * partA, EntityPart * partB );

    void computeAssemblyRQVW();
    void computePartsRQVW();


public:
    void assignIndices();
    void computeCenterOfInertia();
    void cleanup();

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
    std::vector<Connection>   connections;
    AssembliesManager * assembliesMgr;
};




}

#endif


