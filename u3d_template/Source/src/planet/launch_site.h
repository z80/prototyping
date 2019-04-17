
#ifndef __LAUNCH_SITE_H_
#define __LAUNCH_SITE_H_

#include <Urho3D/Urho3DAll.h>

namespace Osp
{

using namespace Urho3D;

class LaunchSite: public LogicComponent
{
    URHO3D_OBJECT( LaunchSite, LogicComponent )
public:
    LaunchSite( Context * ctx );
    ~LaunchSite();

    void Start() override;

private:
};

}


#endif





