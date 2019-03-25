
#ifndef __BLOCK_H_
#define __BLOCK_H_

#include "item_base.h"
#include "pivot_marker.h"

using namespace Urho3D;

namespace Urho3D
{
    class RigidBody2;
    class CollisionShape2;
}

namespace Osp
{

class Block: public ItemBase
{
    URHO3D_OBJECT( Block, ItemBase )
public:
    Block( Context * c, const String & name );
    virtual ~Block();

    virtual void createContent( Node * n );

    RigidBody2 * rigidBody();
    CollisionShape2 * collisionShape();
    void setPivotsVisible( bool en );
    void setPivotSize( float sz );
    /// Blocks are in a tree with "root" as
    /// a common parent. Tries all own pivot points
    /// and finds the closest pivot point on another
    /// object. Returns the object with the closest
    /// pivot point.
    Block * tryAttach();
    bool    detach();

    Block * parentBlock();

    const Vector3 axisToParent();

public:
    /// Each object implementation
    /// places markers individually.
    void placePivots();
    void clearPivots();
    void createPivots( size_t qty );
    /// Align orientation after attaching the object to it's parent.
    void alignOrientation( const Vector3 & ownA, const Vector3 & parentA );
    /// Attaching functionality.
    Block * tryAttachToConnectionPoint();
    Block * tryAttachToSurface();


    String name;
    std::vector< SharedPtr<PivotMarker> > pivots;
protected:
    virtual void OnNodeSet( Node * node );
};

}




#endif




