
#ifndef __DESIGN_CONSTRUCTION_H_
#define __DESIGN_CONSTRUCTION_H_

#include "Ogre.h"
#include "tech_tree_panel.h"

namespace Entity
{


class DesignConstruction: public TechTreePanelCallback
{
public:
    DesignConstruction();
    ~DesignConstruction();

    void part( const Ogre::String & name );

public:
};


}


#endif



