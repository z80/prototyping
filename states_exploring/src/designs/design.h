
#ifndef __DESIGN_H_
#define __DESIGN_H_

#include "entity_part.h"

namespace Osp
{


struct Connection
{
    // Additional axis for rotation is not needed.
    // It is supposed to be fixed by part design.
    // So really need only "r" and "q".
    Ogre::int32      blockA;
    Ogre::int32      blockB;
    Ogre::Vector3    r;
    Ogre::Quaternion q;
};

class Design
{
public:
    Design();
    ~Design();

    Design( const Design & inst );
    const Design & operator=( const Design & inst );

    bool save( const Ogre::String & fname, bool overwrite=false );
    bool load( const Ogre::String & fname );
    /// 1) Each part is in joints at least once.
    /// 2) Joints connect existing parts.
    /// 3) No isolated components.
    bool valid() const;

public:
    std::vector<Ogre::String> parts;
    std::vector<Connection>   joints;
};



}


#endif



