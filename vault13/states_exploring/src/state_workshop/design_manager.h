
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
        int          id;
    };

    DesignManager();
    ~DesignManager();

    bool saveDesign( const Ogre::String & name,
                     const Ogre::String & description,
                     const Design & design );
    Design loadDesign( int index );
    bool renameDesign( int index,
                       const Ogre::String & nameNew );
    void setDescription( int index,
                         const Ogre::String & description );
    void destroyDesign( int index );
    std::vector<Ogre::String> designNames();
    const DesignItem & designItem( int ind );


public:
    Ogre::String lastErr;

    std::vector<DesignItem> designs;
};


}

#endif



