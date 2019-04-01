
#ifndef __ONE_PLANET_MODE_H_
#define __ONE_PLANET_MODE_H_

#include "mode_base.h"
#include "tech_tree.h"
#include "block.h"
#include "design_manager.h"

namespace Urho3D
{
    class PhysicsWorld2;
}

namespace Osp
{

class OnePlanetMode: public ModeBase
{
URHO3D_OBJECT( OnePlanetMode, ModeBase )

public:
    /// Construct.
    OnePlanetMode( Context * context );

    ~OnePlanetMode();

    void activate()   override;
    void deactivate() override;


private:
    void CreateScene();
    void CreateUI();
    void SubscribeToEvents();


    SharedPtr<Node>      rootNode;
    SharedPtr<UIElement> workshopPnl;
    SharedPtr<PhysicsWorld> physicsWorld;
    int mouseX,     mouseY,
        mousePrevX, mousePrevY;
public:
    void createObjects();
    void createDesign();

    bool select();

    void HandleUpdate( StringHash t, VariantMap & e );
    void HandlePostRenderUpdate( StringHash t, VariantMap & e );
    // Subscribe to fixed timestep physics updates for setting or applying controls
    void HandlePhysicsPreStep( StringHash t, VariantMap & e );
    // Subscribe HandlePostUpdate() method for processing update events. Subscribe to PostUpdate instead
    // of the usual Update so that physics simulation has already proceeded for the frame, and can
    // accurately follow the object with the camera
    void HandlePostUpdate( StringHash t, VariantMap & e );

    // Keyboard and mouse input for editor.
    void HandleMouseDown( StringHash t, VariantMap & e );
    void HandleMouseUp( StringHash t, VariantMap & e );
    void HandleMouseMove( StringHash t, VariantMap & e );
    void HandleKeyDown( StringHash t, VariantMap & e );
    void HandleKeyUp( StringHash t, VariantMap & e );

    // Change to workshop.
    void HandleToWorkshop( StringHash t, VariantMap & e );
};

}


#endif


