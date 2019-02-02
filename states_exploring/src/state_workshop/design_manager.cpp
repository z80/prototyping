
#include "design_manager.h"
#include "tinyxml2.h"

namespace Osp
{

static bool designNameExists( DesignManager & dm, const Ogre::String & name );
static bool loadDesigns( DesignManager & dm );
static bool saveDesigns( DesignManager & dm );
static bool loadDesign( DesignManager & dm, int index );
static bool saveDesign( DesignManager & dm, int index );
static int  uniqueId( DesignManager & dm );

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

bool DesignManager::renameDesign( int index,
                                  const Ogre::String & nameNew )
{
    designs[index].name = nameNew;
    return true;
}

void DesignManager::setDescription( int index,
                                    const Ogre::String & description )
{
    designs[index].desc = description;
    return true;
}

void DesignManager::destroyDesign( int index )
{
    const int qty = (int)designs.size();
    const int lastInd = qty-1;
    if ( index < lastInd )
        designs[i] = designs[lastInd];
    designs.resize( lastInd );
    ::saveDesigns( *this );
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
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement * rootE = doc.NewElement( "designs" );
    const size_t qty = designs.size();
    for ( size_t i=0; i<qty; i++ )
    {
        const DesignManager::DesignItem & di = designs[i];
        tinyxml2::XMLElement * designE = doc.NewElement( "design" );
        designE->SetAttribute( "name", di.name.c_str() );
        {
            std::ostringstream out;
            out << di.id;
            designE->SetAttribute( "id", out.str().c_str() );
        }
        designE->SetText( di.desc );
    }
    doc.SaveFile( "./designs.xml" );
    return true;
}

static bool loadDesign( DesignManager & dm, int index )
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile( "./designs.xml" );
    dm.designs.clear();

    tinyxml2::XMLElement * rootE = doc.FirstChildElement( "designs" );
    while ( true )
    {
        tinyxml2::XMLElement * designE;
        if ( dm.designs.empty() )
            designE = rootE->FirstChildElement();
        else
            designE = designE->NextSiblingElement();
        if ( !designE )
            break;

        DesignManager::DesignItem di;
        di.name = designE->Attribute( "name" );
        {
            std::istringstream in( designE->Attribute( "id" ) );
            in >> di.id;
        }
        di.desc = designE->GetText();
        dm.designs.push_back( di );
    }

    return true;
}

static bool saveDesign( DesignManager & dm, int index )
{
    return false;
}

static int  uniqueId( DesignManager & dm )
{
    std::set<int> ids;
    const size_t qty = designs.size();
    for ( size_t i=0; i<qty; i++ )
    {
        const DesignManager::DesignItem & di = designs[i];
        ids.insert( di.id );
    }

    int id = 0;
    while ( true )
    {
        std::set<int>::const_iterator it = ids.find( id );
        if ( it == ids.end() )
            break;
        id += 1;
    }

    return id;
}




}




