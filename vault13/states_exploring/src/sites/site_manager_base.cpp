
#include "site_manager_base.h"
#include "site.h"

namespace Osp
{

SiteManagerBase::SiteManagerBase()
{
}

SiteManagerBase::~SiteManagerBase()
{
    destroy();
}

void SiteManagerBase::create()
{
    createImpl();
}

int SiteManagerBase::sitesQty() const
{
    const int qty = static_cast<int>( sites.size() );
    return qty;
}

int SiteManagerBase::sitesQty( const Ogre::String & function ) const
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

Site * SiteManagerBase::site( int index )
{
    Site * s = sites[index];
    return s;
}


Site * SiteManagerBase::site( const Ogre::String & function, const Ogre::String & name )
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

void SiteManagerBase::destroy()
{
    const size_t qty = sites.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Site * s = sites[i];
        delete s;
    }
    sites.clear();
}

bool SiteManagerBase::setCurrent( const Ogre::String & function, const Ogre::String & name )
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

Site * SiteManagerBase::current( const Ogre::String & function )
{
    const size_t qty = mSelected.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Current & c = mSelected[i];
        if ( c.function == function )
        {
            const int ind = c.index;
            Site * s = sites[ind];
            return s;
        }
    }

    return 0;
}





}




