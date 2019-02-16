
#include "pivot_marker.h"
#include "name_generator.h"

using namespace Urho3D;

namespace Osp
{

PivotMarker::PivotMarker( Context * c )
    : ItemBase( c )
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();

    Scene * scene = GetScene();
    // Create the scene node & visual representation. This will be a replicated object
    modelNode = scene->CreateChild( NameGenerator::Next( "Pivot" ) );
    model     = modelNode->CreateComponent<StaticModel>();
    model->SetModel( cache->GetResource<Model>("Models/Sphere.mdl") );
    model->SetMaterial( cache->GetResource<Material>("Materials/StoneSmall.xml") );
}

PivotMarker::~PivotMarker()
{

}

void PivotMarker::setSize( float sz )
{
    modelNode->SetScale( sz );
}

}

