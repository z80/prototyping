
#include "game_data.h"

namespace Osp
{

GameData::GameData( Context * ctx )
    :Object( ctx )
{

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


