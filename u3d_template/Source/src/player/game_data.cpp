
#include "game_data.h"

namespace Osp
{

// Time in microseconds.
const Float GameData::ONE_SECOND  = 1000000;
const Float GameData::_ONE_SECOND = 1.0e-6;

GameData::GameData( Context * ctx )
    :Object( ctx )
{
    time = 0;
}

GameData::~GameData()
{

}

SharedPtr<Node> GameData::node( const String & name )
{
    HashMap<String,SharedPtr<Node> >::ConstIterator it = nodes.Find( name );
    if ( it == nodes.End() )
        return SharedPtr<Node>();
    return it->second_;
}


}


