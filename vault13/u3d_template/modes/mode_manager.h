
#ifndef __MODE_MANAGER_H_
#define __MODE_MANAGER_H_

#include <Urho3D/Urho3DAll.h>
#include "mode_base.h"

using namespace Urho3D;

namespace Osp
{

class ModeManager: public Urho3D::Component
{
    URHO3D_OBJECT( ModeManager, Component )
public:
    ModeManager( Context * ctx );
    ~ModeManager();

    static void RegisterObject( Urho3D::Context * ctx );

    void activateMode( StringHash t, VariantMap & args );
public:
    void cleanup();

    SharedPtr<ModeBase> activeMode;
    Vector< SharedPtr<ModeBase> > modes;
};

}



#endif


