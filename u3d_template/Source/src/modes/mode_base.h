
#ifndef __MODE_NAME_H_
#define __MODE_NAME_H_

#include <Urho3D/Urho3DAll.h>

using namespace Urho3D;

namespace Osp
{

class ModeBase: public Urho3D::LogicComponent
{
    URHO3D_OBJECT( ModeBase, LogicComponent )
public:
    ModeBase( Context * ctx );
    ~ModeBase();

    const String & name() const;

    virtual void activate();
    virtual void deactivate();

    // Overwrite vidtual methods.
    void DelayedStart() override;
    void Stop() override;

    bool isActive() const;
protected:
    String name_;
    bool   active_;
};


}


#endif


