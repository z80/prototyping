
#ifndef __DESIGN_H_
#define __DESIGN_H_

#include "entity_part.h"

namespace Entity
{


struct ConnectionDesc
{
    // Additional axis for rotation is not needed.
    // It is supposed to be fixed by part design.
    // So really need only "r" and "q".
    Ogre::int32      partA;
    Ogre::int32      partB;
    Ogre::Vector3    r;
    Ogre::Quaternion q;
};

class Design
{
public:
    Design();
    ~Design();

    bool save( const Ogre::String & fname, bool overwrite=false );
    bool load( const Ogre::String & fname );
    /// 1) Each part is in joints at least once.
    /// 2) Joints connect existing parts.
    bool valid() const;

public:
    std::vector<Ogre::String>    parts;
    std::vector<ConnectionDesc> joints;
};



}


#endif



