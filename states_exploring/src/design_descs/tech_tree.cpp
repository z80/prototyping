
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
    sm->pushFunc( "initTechTree" );
    const bool callOk = sm->callFunc();

    return callOk;
}

void TechTree::generatePanelContent()
{
    // Clean up allowed parts.
    const size_t catsQty = panelContent.size();
    for ( size_t i=0; i<catsQty; i++ )
    {
        CategoryDesc & c = panelContent[i];
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
                    CategoryDesc & c = panelContent[k];
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

bool TechTree::enableNode( const Ogre::String & name )
{
    const size_t nodesQty = nodes.size();
    for ( size_t j=0; j<nodesQty; j++ )
    {
        TechNode & n = nodes[j];
        if ( n.name == name )
        {
            // Check that all parents are enabled as well.
            const size_t ptsQty = n.parents.size();
            for ( size_t j=0; j<ptsQty; j++ )
            {
                const bool parentEnabled = nodeEnabled( n.parents[j] );
                if ( !parentEnabled )
                    return false;
            }
            n.enabled = true;
            return true;
        }
    }

    return false;
}

const std::vector<CategoryDesc> & TechTree::getPanelContent() const
{
    return panelContent;
}

const std::vector<PartDesc> & TechTree::getPartDescs() const
{
    return partDescs;
}

bool TechTree::nodeEnabled( const Ogre::String & name ) const
{
    const size_t nodesQty = nodes.size();
    for ( size_t j=0; j<nodesQty; j++ )
    {
        const TechNode & n = nodes[j];
        if ( n.name == name )
        {
            return n.enabled;
        }
    }

    return false;
}


}



