
#include <Urho3D/Urho3DAll.h>

namespace Osp
{

using namespace Urho3D;

/**
 * @brief The Entity class
 * This is the base class for all interacting objects.
 */
class CameraOrb2: public Component
{
    URHO3D_OBJECT( CameraOrb2, Component )
public:
    CameraOrb2( Context * c );
    ~CameraOrb2();

    void SubscribeToEvents();

    void HandleMouseDown( StringHash t, VariantMap & e );
    void HandleMouseUp( StringHash t, VariantMap & e );
    void HandleMouseMove( StringHash t, VariantMap & e );
    void HandleMouseWheel( StringHash t, VariantMap & e );

    void updateCamera();
private:
    float az, el, r;
    float va, vr;
    bool  activated;
};

}


