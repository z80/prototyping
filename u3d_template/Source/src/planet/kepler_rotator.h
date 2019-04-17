
#ifndef __KEPLER_ROTATOR_H_
#define __KEPLER_ROTATOR_H_

#include <Urho3D/Urho3DAll.h>
#include "planet_base.h"
#include "game_data.h"
#include "data_types.h"

namespace Osp
{

class KeplerRotator: public ItemBase
{
    URHO3D_OBJECT( KeplerRotator, ItemBase )
public:
    KeplerRotator( Context * ctx );
    ~KeplerRotator();

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


