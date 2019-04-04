
#ifndef __GAME_DATA_H_
#define __GAME_DATA_H_

#include <Urho3D/Urho3DAll.h>

#include "assembly.h"
#include "block.h"

namespace Osp
{

using namespace Urho3D;



class GameData: public Urho3D::Object
{
    URHO3D_OBJECT( GameData, Object )
public:
    GameData( Context * ctx );
    ~GameData();

    SharedPtr<Node> node( const String & name );
    Design design;
private:
    HashMap<String, SharedPtr<Node> > nodes;
};

}

#endif


