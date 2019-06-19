
#include "game_data.h"

namespace Osp
{

// Time in microseconds.
const Timestamp GameData::ONE_SECOND  = 1000000;
const Float     GameData::_ONE_SECOND = 1.0e-6;
const Float     GameData::DIST_PLANET_COLLISIONS = 1000.0;
const Float     GameData::DIST_WORLD_ENTER      = 500.0;
const Float     GameData::DIST_WORLD_LEAVE      = 700.0;
const Float     GameData::DIST_WHEN_NEED_MOVE   = 5.0;
const Float     GameData::MIN_ANGULAR_MOMENTUM  = 0.0001;

GameData::GameData( Context * ctx )
    :LogicComponent( ctx )
{
    time = 0;
}

GameData::~GameData()
{

}

void GameData::Update( float dtf )
{
    dt = static_cast<Timestamp>(
             static_cast<Float>(dtf) *
             static_cast<Float>(ONE_SECOND)
         );
    time += dt;
}

SharedPtr<Node> GameData::node( const String & name )
{
    HashMap<String,SharedPtr<Node> >::ConstIterator it = nodes.Find( name );
    if ( it == nodes.End() )
        return SharedPtr<Node>();
    return it->second_;
}


}


