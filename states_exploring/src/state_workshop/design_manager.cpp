
#include "design_manager.h"
#include "tinyxml2.h"

namespace Osp
{

static bool designNameExists( DesignManager & dm, const Ogre::String & name );
static bool loadDesigns( DesignManager & dm );
static bool saveDesigns( DesignManager & dm );
static bool loadDesign( DesignManager & dm, int index );
static bool saveDesign( DesignManager & dm, int index );

DesignManager::DesignManager()
{
    ::loadDesigns( *this );
}

DesignManager::~DesignManager()
{
    ::saveDesigns( *this );
}

bool DesignManager::saveDesign( const Ogre::String & name,
                                const Ogre::String & description,
                                const Design & design )
{
    DesignItem di;
    di.name = name;
    di.desc = description;
    const int index = (int)designs.size();
    designs.push_back( di );
    // And save design file to disk.
    // tbd...
    const bool savedOk = ::saveDesign( *this, index );
    ::saveDesigns( *this );
    return savedOk;
}

bool DesignManager::renameDesign( const Ogre::String & name,
                                  const Ogre::String & nameNew )
{
    const int qty = (int)designs.size();
    for ( int i=0; i<qty; i++ )
    {
        const Ogre::String & n = designs[i].name;
        if ( name == n )
        {
            designs[i].name = nameNew;
            ::saveDesigns( *this );
            return;
        }
    }
}

void DesignManager::setDescription( const Ogre::String & name,
                                    const Ogre::String & description )
{
    const int qty = (int)designs.size();
    for ( int i=0; i<qty; i++ )
    {
        const Ogre::String & n = designs[i].name;
        if ( name == n )
        {
            designs[i].desc = description;
            ::saveDesigns( *this );
            return;
        }
    }
}

void DesignManager::destroyDesign( const Ogre::String & name )
{
    const int qty = (int)designs.size();
    const int lastInd = qty-1;
    for ( int i=0; i<qty; i++ )
    {
        const Ogre::String & n = designs[i].name;
        if ( name == n )
        {
            if ( i < lastInd )
                designs[i] = designs[lastInd];
            designs.resize( lastInd );
            ::saveDesigns( *this );
            return;
        }
    }
}

std::vector<Ogre::String> DesignManager::designs()
{
    const int qty = (int)designs.size();
    std::vector<Ogre::String> names;
    names.reserve( qty );
    for ( int i=0; i<qty; i++ )
        names.push_back( designs[i].name );

    return names;
}

bool DesignManager::design( int ind, DesignManager::DesignItem & d )
{
    const int qty = (int)designs.size();
    if ( ind >= qty )
        return false;
    d = designs[ind];
    return true;
}

bool DesignManager::design( int ind, Design & d )
{
    const bool loadedOk = d.load( "aaa" );
    return loadedOk;
}






static bool designNameExists( DesignManager & dm,
                              const Ogre::String & name )
{
    using namespace DesignManager;
    const std::vector<DesignItem> & designs = dm.designs;
    const size_t qty = designs.size();
    for ( size_t i=0; i<qty; i++ )
    {
        const DesignItem & di = designs[i];
        if ( di.name == name )
            return true;
    }

    return false;
}

static bool loadDesigns( DesignManager & dm )
{
    return false;
}

static bool saveDesigns( DesignManager & dm )
{
    return false;
}

static bool loadDesign( DesignManager & dm, int index )
{
    return false;
}

static bool saveDesign( DesignManager & dm, int index )
{
    return false;
}




}




