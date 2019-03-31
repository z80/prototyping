
#include "mode_manager.h"
#include "MyEvents.h"

#include "mode_workshop.h"

namespace Osp
{

ModeManager::ModeManager( Context * ctx )
    : Component( ctx )
{
    SubscribeToEvent( MyEvents::E_SETMODE, URHO3D_HANDLER( ModeManager, activateMode ) );
}

ModeManager::~ModeManager()
{
    cleanup();
}

void ModeManager::RegisterObject( Urho3D::Context * ctx )
{
    ctx->RegisterFactory<ModeManager>();
    ctx->RegisterFactory<WorkshopMode>();
}

void ModeManager::activateMode( StringHash t, VariantMap & args )
{
    const Variant & v = args[ "Name" ];
    const String & name = v.GetString();
    // Check if it is already active.
    if ( activeMode )
    {
        const String & activeName = activeMode->name();
        if ( activeName == name )
            return;
    }

    {
        // Make sure that no one died yet.
        while ( true )
        {
            const int qty = (int)modes.Size();
            const int lastInd = qty - 1;
            for ( int i=0; i<qty; i++ )
            {
                SharedPtr<ModeBase> & m = modes[i];
                if ( (!m) && (i!= lastInd) )
                {
                    modes[i] = modes[lastInd];
                    modes.Resize( lastInd );
                    continue;
                }
            }
            break;
        }
    }

    // If reached here it is not active.
    // Search it in created modes.
    const int qty = (int)modes.Size();
    const int lastInd = qty - 1;
    for ( int i=0; i<qty; i++ )
    {
        SharedPtr<ModeBase> & m = modes[i];
        const String & inactiveName = m->name();
        if ( inactiveName == name )
        {
            SharedPtr<ModeBase> buf = m;
            if ( activeMode )
            {
                activeMode->deactivate();
                modes[i] = activeMode;
            }
            else
            {
                if ( i!=lastInd )
                {
                    modes[i] = modes[lastInd];
                    modes.Resize(lastInd);
                }
            }
            buf->activate();
            activeMode = buf;
            return;
        }
    }

    // If reached this point mode isn't created yet.
    Scene * s = GetScene();
    Component * c = s->CreateComponent( StringHash(name) );
    Scene * ss = c->GetScene();
    SharedPtr<ModeBase> m( c->Cast<ModeBase>() );
    if ( m )
    {
        if ( activeMode )
        {
            activeMode->deactivate();
            modes.Push( activeMode );
        }
        // Here no need in activation.
        // When created it is activated itself.
        activeMode = m;
    }
}

void ModeManager::cleanup()
{
    if ( activeMode )
        activeMode->Remove();
    const size_t qty = modes.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        SharedPtr<ModeBase> m = modes[i];
        if ( m )
            m->Remove();
    }
    modes.Clear();
}


}


