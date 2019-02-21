
#ifndef __WORKSHOP_H_
#define __WORKSHOP_H_

#include "BaseLevel.h"
#include "tech_tree.h"
#include "block.h"

namespace Osp
{

class Workshop: public BaseLevel
{
URHO3D_OBJECT( Workshop, BaseLevel )

public:
    enum Mode { None, Drag, Rotate };

    /// Construct.
    Workshop( Context * context );

    ~Workshop();

protected:
    void Init() override;

private:
    void CreateScene();
    void CreateUI();
    void createSectionsUi();
    void createBlocksUi( int groupInd );
    void createModeUi();

    void SubscribeToEvents();

    Button * CreateButton( const String & text, int width, IntVector2 position );

    SharedPtr<UIElement> _uiRoot;
    SharedPtr<Window>    _panelMain,
                         _panelGroups,
                         _panelBlocks;
    SharedPtr<Text>      _modeText;

    SharedPtr<Node>  rootNode;
    SharedPtr<Block> selectedBlock;
    Mode             mode;
    int mouseX, mouseY;

    SharedPtr<TechTree> techTree;
public:
    bool select();
    void cameraPlane( Vector3 & x, Vector3 & y, Vector3 & n );
    void mouseIntersection( Vector3 & at, const Vector3 & origin=Vector3::ZERO );

    void hintDefault();
    void hintSelected();
    void hintDragged();
    void hintRotated();

    void windowBlockParams();
    void createAuxilaryPanel();

    void showPivots( bool en );
    void drag();
    void dragStart();
    void dragStop();
    void rotate();
    void rotateStart();
    void rotateStop();



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


    /// UI events processing.
    /// Blocks panel.
    void HandlePanelGroupClicked( int ind );
    void HandlePanelBlockClicked( const String name );
    void HandlePanelGroupSelected( StringHash eventType, VariantMap & eventData );
    void HandlePanelBlockSelected( StringHash eventType, VariantMap & eventData );

    /// UI Save/load design.
    void HandleSaveDesignDialog( StringHash eventType, VariantMap & eventData );
    void HandleLoadDesignDialog( StringHash eventType, VariantMap & eventData );
};

}


#endif


