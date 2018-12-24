
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

    void integrateDynamics( Ogre::Real t_sec );
    void setParent( EntityPlanet * planet );
    void setParentRf( EntityPlanet * planet );

    void destroyPart( EntityPart * part );

    void computeRQ();

    Ogre::Vector3     r;
    Ogre::Quaternion  q;
    bool              onSurface;
    EntityPlanet    * parent;
    Ogre::SceneNode * sceneNode;

    std::vector<EntityPart *> part;
    AssembliesManager * assembliesMgr;
};




}

#endif


