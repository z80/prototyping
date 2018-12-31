
#ifndef __LAUNCH_SITE_MANAGER_BASE_H_
#define __LAUNCH_SITE_MANAGER_BASE_H_

#include "Ogre.h"

namespace Entity
{

class Site;

class LaunchSiteManagerBase
{
public:
    LaunchSiteManagerBase();
    virtual ~LaunchSiteManagerBase();

    virtual void create();

    int sitesQty() const;
    int sitesQty( const Ogre::String & function ) const;
    Site * site( int index );
    Site * site( const Ogre::String & function, const Ogre::String & name );

    bool setCurrent( const Ogre::String & function, const Ogre::String & name );
    Site * current( const Ogre::String & function );


public:
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



