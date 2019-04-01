
#include "level_main.h"
#include "Urho3D/Core/CoreEvents.h"
#include "Urho3D/Input/InputEvents.h"
#include "Urho3D/UI/UIEvents.h"

#include "Global.h"
#include "MyEvents.h"
#include "Audio/AudioManagerDefs.h"
#include "Messages/Achievements.h"
#include "Input/ControllerInput.h"
#include "LevelManager.h"

#include "name_generator.h"
#include "design_manager.h"
#include "block.h"
#include "assembly.h"
#include "camera_orb_2.h"
#include "mode_manager.h"

/*
#include "physics_world_2.h"
#include "collision_shape_2.h"
#include "rigid_body_2.h"
#include "constraint_2.h"
#include "physics_events_2.h"
*/
#include "Urho3D/Physics/PhysicsWorld.h"
#include "Urho3D/Physics/CollisionShape.h"
#include "Urho3D/Physics/RigidBody.h"
#include "Urho3D/Physics/Constraint.h"
#include "Urho3D/Physics/PhysicsEvents.h"


#include <iostream>

using namespace Urho3D;

namespace Osp
{


LevelMain::LevelMain(Context* context)
    : BaseLevel(context)
{
}

LevelMain::~LevelMain()
{
}

void LevelMain::Init()
{
    if (!scene_)
    {
        // There is no scene, get back to the main menu
        VariantMap& eventData = GetEventDataMap();
        eventData["Name"] = "MainMenu";
        SendEvent(MyEvents::E_SET_LEVEL, eventData);

        return;
    }
    scene_->RemoveAllChildren();


    {
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        XMLFile * f = cache->GetResource<XMLFile>( "Scenes/Workshop.xml" );
        if ( !f )
            return;
        Node * n = scene_->CreateChild( "Workshop" );
        const bool loadedOk = scene_->LoadXML( f->GetRoot() );
        scene_->RemoveAllChildren();
    }


    BaseLevel::Init();

    // Disable achievement showing for this level
    GetSubsystem<Achievements>()->SetShowAchievements(true);

    // Create the scene content
    CreateScene();

    // Create game mode.
    CreateFirstMode();
}

void LevelMain::CreateScene()
{
    Input * inp = GetSubsystem<Input>();
    inp->SetMouseVisible( true );

    ControllerInput * controllerInput = GetSubsystem<ControllerInput>();
    Vector<int> controlIndexes = controllerInput->GetControlIndexes();
    InitViewports(controlIndexes);

    // Just in case set camera node name explicitly.
    Node * camNode = _cameras[0];
    camNode->SetName( "MainCamera" );
}

void LevelMain::CreateFirstMode()
{
    Scene * s = scene_;
    modeManager = s->CreateComponent<ModeManager>();

    VariantMap& eventData = GetEventDataMap();
    eventData["Name"] = "WorkshopMode";
    SendEvent( MyEvents::E_SETMODE, eventData );
}





}


