
#ifndef __DESIGN_H_
#define __DESIGN_H_

#include "entity_part.h"

namespace Osp
{




class Design
{
public:
    struct Joint
    {
        // Additional axis for rotation is not needed.
        // It is supposed to be fixed by part design.
        // So really need only "r" and "q".
        Ogre::int32      blockA;
        Ogre::int32      blockB;
    };

    struct Block
    {
        // Block type name to be able to create one.
        Ogre::String name;
        // Position and orientation with respect to
        // the Site design was made in.
        Ogre::Vector3    r;
        Ogre::Quaternion q;
    };

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
    std::vector<Block> blocks;
    std::vector<Joint> joints;
};



}


#endif



