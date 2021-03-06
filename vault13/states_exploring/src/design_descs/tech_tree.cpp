
#include "tech_tree.h"
#include "config_reader.h"
#include "state_manager.h"
#include "lua.hpp"

namespace Osp
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

bool TechTree::generatePanelContent()
{
    const bool loadedOk = load( StateManager::getSingletonPtr()->getConfigReader() );
    if ( !loadedOk )
        return false;

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

    return true;
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

const PartDesc & TechTree::partDesc( const Ogre::String & name )
{
    const size_t partsQty = partDescs.size();
    for ( size_t i=0; i<partsQty; i++ )
    {
        const PartDesc & pd = partDescs[i];
        if ( pd.name == name )
            return pd;
    }
    static PartDesc failureDesc;
    return failureDesc;
}

std::vector<CategoryDesc> & TechTree::getPanelContent()
{
    return panelContent;
}

std::vector<PartDesc> & TechTree::getPartDescs()
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



