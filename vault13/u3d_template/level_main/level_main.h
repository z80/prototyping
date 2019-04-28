
#ifndef __LEVEL_MAIN_H_
#define __LEVEL_MAIN_H_

#include "BaseLevel.h"
#include "mode_manager.h"

namespace Urho3D
{
    class PhysicsWorld2;
}

namespace Osp
{

class LevelMain: public BaseLevel
{
URHO3D_OBJECT( LevelMain, BaseLevel )

public:
    /// Construct.
    LevelMain( Context * context );

    ~LevelMain();

protected:
    void Init() override;

private:
    void CreateScene();
    void CreateFirstMode();

    SharedPtr<ModeManager> modeManager;
};

}


#endif


