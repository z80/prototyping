
#ifndef __PLANET_BASE_H_
#define __PLANET_BASE_H_

#include <Urho3D/Urho3DAll.h>
#include "item_base.h"
#include "block.h"
#include "assembly.h"
#include "data_types.h"

namespace Osp
{

using namespace Urho3D;

class PlanetBase: public ItemBase
{
    URHO3D_OBJECT( PlanetBase, ItemBase )
public:
    PlanetBase( Context * ctx );
    virtual ~PlanetBase();

    virtual void Update( float timeStep );

protected:
    bool tryAddSurfaceItem( Node * n );
    bool tryAddOrbitingItem( Node * n );
    void subscribeToParentChanges();
    void OnNodeRemoved( StringHash eventType, VariantMap & eventData );
    void OnNodeAdded( StringHash eventType, VariantMap & eventData );

    // There are at least 3 nodes.
    // 1-st translating, moving the orbit.
    // 2-d  attached to the 1-st one but also rotating. Physcis world and collision bodies
    // should be attached to this one. And atmosphere effects should also be taken into
    // account here.
    // 3-d, 4-th, etc. for graphics.
    // Need only a pointer to the 2-d one here as 1-st one can be obtained by "GetNode()".
    SharedPtr<Node>  dynamicsNode;
    // Yes, can obtain all these through "getComponents" recursive call but will spend
    // some time on casting types in this case.
    Vector<Block>    surfaceBlocks,
                     orbitingBlocks;
    Vector<Assembly> orbitingAssemblies;

    typedef double Float;
    static const Float subinterval_;
};

}



#endif

