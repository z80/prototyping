
#ifndef __BLOCK_H_
#define __BLOCK_H_

#include "item_base.h"
#include "pivot_marker.h"
#include "air_mesh.h"

using namespace Urho3D;

namespace Urho3D
{
    class RigidBody2;
    class CollisionShape2;
}

namespace Osp
{

struct ForceApplied
{
    Vector3d at;
    Vector3d F;
};

class Block: public ItemBase
{
    URHO3D_OBJECT( Block, ItemBase )
public:
    Block( Context * c, const String & name );
    virtual ~Block();

    void Start() override;

    virtual void createContent( Node * n );

    void drawDebugGeometry( DebugRenderer * debug );

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

    const Vector3d axisToParent();

public:
    /// Each object implementation
    /// places markers individually.
    void placePivots();
    void clearPivots();
    void createPivots( size_t qty );
    /// Align orientation after attaching the object to it's parent.
    void alignOrientation( const Vector3d & ownA, const Vector3d & parentA );
    /// Attaching functionality.
    Block * tryAttachToConnectionPoint();
    Block * tryAttachToSurface();
    /// Draw forces applied.
    void drawDebugForces( DebugRenderer * debug );


    /// Need to be able to open configuration window.
    virtual SharedPtr<UIElement> configWindow();

    String name;
    std::vector< SharedPtr<PivotMarker> > pivots;
    AirMesh airMesh;

    // Explicit forces to be able to draw those.
    // Forces applied.
    ForceApplied         gravity;
    Vector<ForceApplied> friction;
};

}




#endif




