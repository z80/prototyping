
#ifndef __GAME_DATA_H_
#define __GAME_DATA_H_

#include <Urho3D/Urho3DAll.h>

#include "assembly.h"
#include "block.h"
#include "data_types.h"

namespace Osp
{

using namespace Urho3D;



class GameData: public Urho3D::LogicComponent
{
    URHO3D_OBJECT( GameData, LogicComponent )
public:
    GameData( Context * ctx );
    ~GameData();

    void Update( float dt ) override;

    SharedPtr<Node> node( const String & name );
    Design    design;
    Timestamp time;
    static const Timestamp ONE_SECOND;
    static const Float    _ONE_SECOND;
private:
    HashMap<String, SharedPtr<Node> > nodes;
};

}

#endif


