
#ifndef __ITEM_BASE_H_
#define __ITEM_BASE_H_

#include <Urho3D/Urho3DAll.h>

namespace Osp
{

using namespace Urho3D;

/**
 * @brief The Entity class
 * This is the base class for all interacting objects.
 */
class ItemBase: public Component
{
    URHO3D_OBJECT( ItemBase, Component );
public:
    enum Type { TUnspecified=0, TBlock, TPlanet, TSite };
    ItemBase( Context * context );
    virtual ~ItemBase();

    Type type();

    //static template<typename T> T * cast<T>( Entity * e );

    virtual void setR( const Vector3 & at );
    virtual Vector3 relR() const;
    virtual void setQ( const Quaternion & q );
    virtual Quaternion relQ() const;

    virtual void setParent( ItemBase * parent, bool inheritRotation = true );
    bool isParentOf( ItemBase * item ) const;
    ItemBase * parentItem() const;

    bool relativePose( ItemBase * other, Vector3 & rel_r, Quaternion & rel_q );
public:
    Type            _type;
};


}


#endif




