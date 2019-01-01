
#include "tech_tree.h"
#include "config_reader.h"
#include "state_manager.h"
#include "lua.hpp"

namespace Entity
{

TechTree::TechTree()
{

}

TechTree::~TechTree()
{

}

bool TechTree::load( Config::ConfigReader * cr )
{
    if ( !cr )
        return false;

    StateManager * sm = StateManager::getSingletonPtr();
    sm->pushFunc( "loadTechTree" );
    const bool callOk = sm->callFunc();

    sm->pushFunc( "loadPartDescs" );
    const bool callOk = sm->callFunc();

    return callOk;
}

void TechTree::generatePanelContent()
{
    // Clean up allowed parts.
    const size_t catsQty = panelContent.size();
    for ( size_t i=0; i<catsQty; i++ )
    {
        Category & c = panelContent[i];
        c.items.clear();
    }

    const size_t partsQty = partDescs.size();
    const size_t nodesQty = nodes.size();
    for ( size_t i=0; i<partsQty; i++ )
    {
        const PartDesc & p = partDescs[i];
        for ( size_t j=0; j<nodesQty; j++ )
        {
            const TechNode & n = nodes[j];
            if ( n.name == p.neededNode )
            {
                for ( size_t k=0; k<catsQty; k++ )
                {
                    Category & c = panelContent[k];
                    if ( c.name == p.category )
                    {
                        c.items.push_back( i );
                        break;
                    }
                }
                break;
            }
        }
    }
}

bool TechTree::partAllowed( const Ogre::String & name )
{

}

bool TechTree::enableNode( const Ogre::String & name ) const
{

}



}



