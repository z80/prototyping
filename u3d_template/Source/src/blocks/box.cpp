
#include "box.h"
#include "tech_tree.h"

namespace Osp
{

Box::Box( Context * c )
    : Block( c )
{
    Node * n = GetNode();
    //n->CreateChild();
    StaticModel * obj = n->CreateComponent<StaticModel>();

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    obj->SetModel( cache->GetResource<Model>( "Models/Box.mdl" ) );
    obj->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
}

Box::~Box()
{

}

void Box::placePivots()
{
    TechTree * tt = GetSubsystem( StringHash( "TechTree" ) )->Cast<TechTree>();
}


}


