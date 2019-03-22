
#include "box.h"
#include "tech_tree.h"

namespace Osp
{

Box::Box( Context * c )
    : Block( c, "Box" )
{
}

Box::~Box()
{

}

void Box::createContent( Node * node )
{
    Block::createContent( node );

    //n->CreateChild();
    StaticModel * obj = node->CreateComponent<StaticModel>();

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    obj->SetModel( cache->GetResource<Model>( "Models/Box.mdl" ) );
    obj->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
    obj->SetCastShadows( true );
}


}


