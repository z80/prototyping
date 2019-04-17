
#include "launch_site.h"

namespace Osp
{

LaunchSite::LaunchSite( Context * ctx )
    : LogicComponent( ctx )
{

}

LaunchSite::~LaunchSite()
{

}

void LaunchSite::Start()
{
    Node * n = GetNode();
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "Prefabs/LaunchSite.xml" );
    if ( !f )
        return;
    Node * rootNode = n->CreateChild( "LaunchSiteVisualNode" );
    const bool loadedOk = rootNode->LoadXML( f->GetRoot() );
    if ( !loadedOk )
        URHO3D_LOGERROR( "Failed to load launch site prefab" );
}


}


