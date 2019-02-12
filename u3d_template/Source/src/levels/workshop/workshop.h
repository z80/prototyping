
#ifndef __WORKSHOP_H_
#define __WORKSHOP_H_

#include "BaseLevel.h"


namespace Osp
{

class Workshop: public BaseLevel
{
URHO3D_OBJECT( Workshop, BaseLevel );

public:
    /// Construct.
    Workshop( Context * context );

    ~Workshop();

protected:
    void Init() override;

private:
    void CreateScene();
    void CreateUI();
    void SubscribeToEvents();

    Button * CreateButton( const String & text, int width, IntVector2 position );

    SharedPtr<UIElement> _uiRoot;
    SharedPtr<Window>    _window;
};

}


#endif


