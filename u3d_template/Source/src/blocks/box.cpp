
#include "box.h"
#include "tech_tree.h"
#include "Urho3D/Physics/RigidBody.h"
#include "Urho3D/Physics/CollisionShape.h"

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

void Box::toWorld()
{
    Node * node = GetNode();

    RigidBody * rb = node->CreateComponent<RigidBody>();
    rb->SetMass( 1.0 );

    CollisionShape * cs = node->CreateComponent<CollisionShape>();
    cs->SetBox( Vector3( 1.0, 1.0, 1.0 ) );
}

void Box::fromWorld()
{
    Block::fromWorld();
}



}


