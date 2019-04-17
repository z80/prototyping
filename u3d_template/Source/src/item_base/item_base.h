
#ifndef __ITEM_BASE_H_
#define __ITEM_BASE_H_

#include <Urho3D/Urho3DAll.h>
#include "vector3d.h"
#include "quaterniond.h"

namespace Osp
{

using namespace Urho3D;

/**
 * @brief The Entity class
 * This is the base class for all interacting objects.
 */
class ItemBase: public LogicComponent
{
    URHO3D_OBJECT( ItemBase, LogicComponent )
public:
    enum Type { TUnspecified=0, TBlock, TPlanet, TSite };
    ItemBase( Context * context );
    virtual ~ItemBase();

    Type type();

    //static template<typename T> T * cast<T>( Entity * e );

    virtual void setR( const Vector3d & new_r );
    virtual Vector3d relR() const;
    virtual void setQ( const Quaterniond & new_q );
    virtual Quaterniond relQ() const;
    virtual void setV( const Vector3d & new_v );
    virtual Vector3d relV() const;
    virtual void setW(const Vector3d & new_w );
    virtual Vector3d relW() const;

    Vector3 relRf() const;
    Quaternion relQf() const;
    Vector3 relVf() const;
    Vector3 relWf() const;

    virtual void setParent( ItemBase * parent, bool inheritRotation = true );
    virtual void setParent( Node * parent, bool inheritRotation = true );
    bool isParentOf( ItemBase * item ) const;
    ItemBase * parentItem() const;

    bool relativePose( ItemBase * other, Vector3d & rel_r, Quaterniond & rel_q );
    bool relativePose( ItemBase * other, Vector3 & rel_r, Quaternion & rel_q );
    bool relativePose( Node * other, Vector3d & rel_r, Quaterniond & rel_q );
    bool relativePose( Node * other, Vector3  & rel_r, Quaternion  & rel_q );
    static bool relativePose( Node * n, Node * p, Vector3 & rel_r, Quaternion & rel_q );
public:
    Type            _type;
    Vector3d    r, v, w;
    Quaterniond q;
};


}


#endif




