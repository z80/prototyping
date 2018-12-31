
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

public:
    void destroy();

    std::vector<Site *> sites;
};



}




#endif



