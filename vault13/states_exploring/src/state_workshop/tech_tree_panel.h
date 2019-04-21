
#ifndef __TECH_TREE_PANEL_H_
#define __TECH_TREE_PANEL_H_

#include "Ogre.h"
#include "design_manager.h"

namespace Osp
{

struct PartDesc;
class  DesignConstruction;

class TechTreePanelCallback
{
public:
    TechTreePanelCallback() {}
    virtual ~TechTreePanelCallback() {}

    virtual void block( const Ogre::String & name ) = 0;
};

class TechTreePanel
{
public:
    TechTreePanel();
    ~TechTreePanel();

    /// Load geometry from a config.
    void initPanelGeometry();

    /// Init/update allowed parts list.
    void updateAllowedParts();

    /// Set tooltip for the panel.
    void setTooltip( const Ogre::String & stri );

    void drawBackToGamePanel();
    void drawTechPanel( TechTreePanelCallback * cb );
    void drawTipPanel();
    void drawMainMenu( DesignManager * dm );
    void drawDesignView( DesignManager * dm );
    void drawDesignSave( DesignManager * dm, DesignConstruction * dc );

    const bool isHovered() const;
public:
    int        panelSz;
    int        iconSz;
    int        itemsPerLine;
    Ogre::Real alpha;
    bool       hovered;

    Ogre::String tooltip;

    int  hoveredDesignIndex;
    bool savingDesign;
    Ogre::String name, desc;
    DesignManager::DesignItem designItem;
};

}

#endif


