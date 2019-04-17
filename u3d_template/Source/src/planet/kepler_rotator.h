
#ifndef __KEPLER_ROTATOR_H_
#define __KEPLER_ROTATOR_H_

#include <Urho3D/Urho3DAll.h>
#include "game_data.h"
#include "data_types.h"

namespace Osp
{

class KeplerRotator: public LogicComponent
{
    URHO3D_OBJECT( KeplerRotator, LogicComponent )
public:
    KeplerRotator( Context * ctx );
    ~KepletRotator();

    void Start() override;
    void Update( float dt ) override;
private:
    SharedPtr<GameData>   gameData;
    SharedPtr<PlanetBase> planet;
    Float yaw, pitch, roll;
    Timestamp period;
};

}

#endif


