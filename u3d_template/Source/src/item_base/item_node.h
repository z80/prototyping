
#ifndef __ITEM_NODE_H_
#define __ITEM_NODE_H_

#include "vector3d.h"
#include "quaterniond.h"
#include <Urho3D/Urho3DAll.h>

using namespace Urho3D;

namespace Osp
{

class ItemNode:  public Urho3D::LogicComponent
{
    URHO3D_OBJECT( ItemNode, LogicComponent )
public:
    ItemNode( Context * ctx );
    virtual ~ItemNode();

    void setParent( ItemNode * itemNode );
    ItemNode * parent() const;

    virtual void setR( const Vector3d & r );
    virtual void setQ( const Quaterniond & q );
    Vector3d     relR() const;
    Quaterniond  relQ() const;

    Vector3d     relV() const;
    Vector3d     relW() const;

    virtual void setV( const Vector3d & v );
    virtual void setW( const Vector3d & w );

    bool relativePose( ItemNode * other, Vector3d & rel_r, Quaterniond & rel_q );

public:
    SharedPtr<ItemNode> parent_;
    Vector3d    r_;
    Quaterniond q_;
    Vector3d    v_;
    Vector3d    w_;
};


}

#endif



