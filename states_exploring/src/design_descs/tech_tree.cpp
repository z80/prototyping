
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

    return callOk;
}

void TechTree::generateAvalilableParts()
{

}

bool TechTree::partAllowed( const Ogre::String & name )
{

}

bool TechTree::enableNode( const Ogre::String & name ) const
{

}



}



