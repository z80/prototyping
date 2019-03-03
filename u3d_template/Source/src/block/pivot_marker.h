
#ifndef __PIVOT_MARKER_H_
#define __PIVOT_MARKER_H_

#include "item_base.h"

using namespace Urho3D;

namespace Osp
{

class Block;

class PivotMarker: public ItemBase
{
    URHO3D_OBJECT( PivotMarker, ItemBase );
public:
    PivotMarker( Context * c );
    ~PivotMarker();
    void createContent();

    void setSize( float sz );

    Block * block();
    PivotMarker * markerConnectedTo();
    Block * blockConnectedTo();

public:
    SharedPtr<Node>        modelNode;
    SharedPtr<StaticModel> model;
    SharedPtr<PivotMarker> connectedTo;
    Vector3 axis;
};

}



#endif




