
#ifndef __GAME_DATA_H_
#define __GAME_DATA_H_

#include <Urho3D/Urho3DAll.h>

#include "assembly.h"
//#include "block.h"
#include "data_types.h"

namespace Osp
{

using namespace Urho3D;

class PlanetBase;

class GameData: public Urho3D::LogicComponent
{
    URHO3D_OBJECT( GameData, LogicComponent )
public:
    GameData( Context * ctx );
    ~GameData();

    void Update( float dt ) override;

    SharedPtr<Node> node( const String & name );
    Design    design;
    Timestamp dt;
    Timestamp time;
    static const Timestamp ONE_SECOND;
    static const Float    _ONE_SECOND;

    // All the planets.
    Vector< PlanetBase * > planets;


    /// Distance for collision objects to exist.
    static const Float DIST_PLANET_COLLISIONS;
    /// Distance for assembly to leave dynamics world.
    static const Float DIST_WORLD_ENTER;
    /// Distance for assembly to enter dynamics world.
    static const Float DIST_WORLD_LEAVE;
    /// World mover should displace when selected assembly is this far.
    static const Float DIST_WHEN_NEED_MOVE;

private:
    HashMap<String, SharedPtr<Node> > nodes;
};

}

#endif


