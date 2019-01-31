
#ifndef __DESIGN_MANAGER_H_
#define __DESIGN_MANAGER_H_

#include "Ogre.h"
#include "design.h"

namespace Osp
{



class DesignManager
{
public:
    struct DesignItem
    {
        Ogre::String name;
        Ogre::String desc;
    };

    DesignManager();
    ~DesignManager();

    bool saveDesign( const Ogre::String & name,
                     const Ogre::String & description,
                     const Design & design );
    bool renameDesign( const Ogre::String & name,
                       const Ogre::String & nameNew );
    void setDescription( const Ogre::String & name,
                         const Ogre::String & description );
    void destroyDesign( const Ogre::String & name );
    std::vector<Ogre::String> designs();
    bool design( int ind, DesignItem & d );
    bool design( int ind, Design & d );


public:
    Ogre::String lastErr;

    std::vector<DesignItem> designs;
};


}

#endif



