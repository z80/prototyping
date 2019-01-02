
#ifndef __TECH_TREE_PANEL_H_
#define __TECH_TREE_PANEL_H_

#include "Ogre.h"

namespace Entity
{

struct PartDesc;

class TechTreePanelCallback
{
public:
    TechTreePanelCallback() {}
    virtual ~TechTreePanelCallback() {}

    virtual void part( const Ogre::String & name ) = 0;
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

    void drawBackToGamePanel();
    void drawTechPanel( TechTreePanelCallback * cb );
public:
    int panelSz;
    int iconSz;
    int itemsPerLine;
};

}

#endif


