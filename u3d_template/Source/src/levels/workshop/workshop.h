
#ifndef __WORKSHOP_H_
#define __WORKSHOP_H_

#include "BaseLevel.h"
#include "tech_tree.h"

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
    void createBlocksUi( int groupInd );
    void SubscribeToEvents();

    Button * CreateButton( const String & text, int width, IntVector2 position );

    SharedPtr<UIElement> _uiRoot;
    SharedPtr<Window>    _panelMain,
                         _panelGroups,
                         _panelBlocks;

    TechTree * techTree;
public:
    /// UI events processing.
    /// Blocks panel.
    void HandlePanelGroupClicked( StringHash eventType, VariantMap & eventData );
    void HandlePanelBlockClicked( StringHash eventType, VariantMap & eventData );
};

}


#endif


