
#ifndef __ITEM_NODE_H_
#define __ITEM_NODE_H_

#include "vector3d.h"
#include "quaterniond.h"
#include <Urho3D/Urho3DAll.h>

using namespace Urho3D;

namespace Osp
{

class ItemNode:  public Urho3D::Object
{
    URHO3D_OBJECT( ItemNode, Object )
public:
    ItemNode( Context * ctx );
    virtual ~ItemNode();

    void setParent( ItemNode * itemNode );

    virtual void setR( const Vector3d & r );
    virtual void setQ( const Quaterniond & q );
    Vector3d relR()    const;
    Quaterniond relQ() const;

    bool relativePose( ItemNode * other, Vector3d & r, Quaterniond & q );

public:
    SharedPtr<ItemNode> parent_;
    Vector3d    r_;
    Quaterniond q_;
};


}

#endif



