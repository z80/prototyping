
#include "pivot_marker.h"
#include "name_generator.h"

using namespace Urho3D;

namespace Osp
{

PivotMarker::PivotMarker( Context * c )
    : ItemBase( c )
{
}

void PivotMarker::createContent()
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();

    // Separate node to attach object to. It is because scale might change.
    // And it is necessary to have a node tree with no scales envolved.
    modelNode = GetNode()->CreateChild();
    model     = modelNode->CreateComponent<StaticModel>();
    model->SetModel( cache->GetResource<Model>("Models/Sphere.mdl") );
    model->SetMaterial( cache->GetResource<Material>("Materials/Stone.xml") );
    model->SetCastShadows( true );
}

PivotMarker::~PivotMarker()
{

}

void PivotMarker::setSize( float sz )
{
    modelNode->SetScale( sz );
}

}

