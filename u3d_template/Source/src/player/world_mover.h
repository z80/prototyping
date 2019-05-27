
#ifndef __WORLD_MOVER_H_
#define __WORLD_MOVER_H_

#include "kepler_mover.h"
#include "planet_base.h"
#include "assembly.h"

namespace Osp
{

class Assembly;
class PlanetBase;

class WorldMover: public KeplerMover
{
    URHO3D_OBJECT( WorldMover, KeplerMover )
public:
    WorldMover( Context * ctx );
    ~WorldMover();

    void Start() override;
    void Update( float dt ) override;

    void setR( const Vector3d & new_r ) override;

    void DrawDebugGeometry( DebugRenderer * debug, bool depthTest ) override;

protected:
    void subscribeToEvents();
    // Whenever it is moved a message is sent to everyone.
    // Whenever player selects an assembly a message is sent as well
    // to let this mover know where to displace the world.
    void OnAssemblySelected( StringHash eventType, VariantMap & eventData );

    // Switches to orbit/ground.
    bool needOrbit() const;
    bool needGround() const;
    void switchToOrbiting();
    void switchToGrounding();

    void switchToEvent( Assembly * assembly );
    void switchTo( Assembly * assembly );
    void tryMoveTo();
    void moveToEvent( const Vector3d & r, const Vector3d & dr, const Vector3d & dv );

public:
    SharedPtr<PlanetBase> planet;
    SharedPtr<Assembly>   assembly;
    // Don't need this. As "active" attribute describes that.
    //bool                  inAtmosphere;
};

}





#endif


