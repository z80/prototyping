
#ifndef __BLOCK_H_
#define __BLOCK_H_

#include "item_base.h"
#include "pivot_marker.h"

using namespace Urho3D;

namespace Osp
{

class Block: public ItemBase
{
    URHO3D_OBJECT( Block, ItemBase );
public:
    Block( Context * c );
    virtual ~Block();

    void setPivotsVisible( bool en );
    void setPivotSize( float sz );
    /// Blocks are in a tree with "root" as
    /// a common parent. Tries all own pivot points
    /// and finds the closest pivot point on another
    /// object. Returns the object with the closest
    /// pivot point.
    Block * tryAttach();
    bool    detach();


    /// Each object implementation
    /// places markers individually.
    virtual void placePivots() = 0;
public:
    void createPivots( size_t qty );
    std::vector< SharedPtr<PivotMarker> > pivots;
};

}




#endif




