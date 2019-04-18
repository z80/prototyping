
#include "game_data.h"

namespace Osp
{

// Time in microseconds.
const Timestamp GameData::ONE_SECOND  = 1000000;
const Float     GameData::_ONE_SECOND = 1.0e-6;

GameData::GameData( Context * ctx )
    :LogicComponent( ctx )
{
    time = 0;
}

GameData::~GameData()
{

}

void GameData::Update( float dt )
{
    Timestamp d = static_cast<Timestamp>(
                    static_cast<Float>(dt) *
                    static_cast<Float>(ONE_SECOND)
                  );
    time += d;
}

SharedPtr<Node> GameData::node( const String & name )
{
    HashMap<String,SharedPtr<Node> >::ConstIterator it = nodes.Find( name );
    if ( it == nodes.End() )
        return SharedPtr<Node>();
    return it->second_;
}


}


