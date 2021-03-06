
#ifndef __SITE_MANAGER_BASE_H_
#define __SITE_MANAGER_BASE_H_

#include "Ogre.h"

namespace Osp
{

class Site;

class SiteManagerBase
{
public:
    SiteManagerBase();
    virtual ~SiteManagerBase();

    void create();

    int sitesQty() const;
    int sitesQty( const Ogre::String & function ) const;
    Site * site( int index );
    Site * site( const Ogre::String & function, const Ogre::String & name );

    bool setCurrent( const Ogre::String & function, const Ogre::String & name );
    Site * current( const Ogre::String & function );


public:
    virtual void createImpl() = 0;

    void destroy();

    std::vector<Site *> sites;

    struct Current
    {
        Ogre::String function;
        size_t       index;
    };
    std::vector<Current> mSelected;
};



}




#endif



