
#include "launch_site_manager_base.h"
#include "launch_site.h"

namespace Entity
{

LaunchSiteManagerBase::LaunchSiteManagerBase()
{
    create();
}

LaunchSiteManagerBase::~LaunchSiteManagerBase()
{
    destroy();
}

void LaunchSiteManagerBase::create()
{

}

int LaunchSiteManagerBase::sitesQty() const
{
    const int qty = static_cast<int>( sites.size() );
    return qty;
}

int LaunchSiteManagerBase::sitesQty( const Ogre::String & function ) const
{
    int n = 0;
    const size_t qty = sites.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Site * s = sites[i];
        if ( s->function == function )
            n += 1;
    }

    return n;
}

Site * LaunchSiteManagerBase::site( int index )
{
    Site * s = sites[index];
    return s;
}


Site * LaunchSiteManagerBase::site( const Ogre::String & function, const Ogre::String & name )
{
    const size_t qty = sites.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Site * s = sites[i];
        if ( (s->function == function) && (s->name == name) )
            return s;
    }

    return 0;

}

void LaunchSiteManagerBase::destroy()
{
    const size_t qty = sites.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Site * s = sites[i];
        delete s;
    }
    sites.clear();
}

bool LaunchSiteManagerBase::setCurrent( const Ogre::String & function, const Ogre::String & name )
{
    // First searc if there already is selected site
    // with this function and overwrite if found.
    const size_t qty = mSelected.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Current & c = mSelected[i];
        if ( c.function == function )
        {
            const size_t sitesQty = sites.size();
            for ( size_t j=0; j<sitesQty; j++ )
            {
                Site * s = sites[j];
                if ( (s->function == function) && (s->name == name) )
                {
                    c.index = j;
                    return true;
                }
            }
            return false;
        }
    }

    // No existing one with selected function.
    // Append one.
    const size_t sitesQty = sites.size();
    for ( size_t j=0; j<sitesQty; j++ )
    {
        Site * s = sites[j];
        if ( (s->function == function) && (s->name == name) )
        {
            Current c;
            c.function = function;
            c.index = j;
            mSelected.push_back( c );
            return true;
        }
    }
    return false;

}

Site * LaunchSiteManagerBase::current( const Ogre::String & function )
{

}





}




