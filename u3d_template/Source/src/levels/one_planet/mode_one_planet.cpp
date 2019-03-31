
#include "mode_one_planet.h"
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

URHO3D_EVENT( E_CATEGORY_CLICKED, CategoryClicked )
{
    URHO3D_PARAM( P_INDEX, index ); // category index
}

URHO3D_EVENT( E_CREATE_BLOCK_CLICKED, CreateBlockClicked )
{
    URHO3D_PARAM( P_NAME, name ); // Block type name.
}


OnePlanetMode::OnePlanetMode(Context* context)
    : ModeBase(context),
      mouseX(0),
      mouseY(0),
      mousePrevX(0),
      mousePrevY(0)
{
    name_ = "OnePlanetMode";
}

OnePlanetMode::~OnePlanetMode()
{
}

void OnePlanetMode::activate()
{
    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateUI();

    // Subscribing to events.
    SubscribeToEvents();
}

void OnePlanetMode::deactivate()
{

}

void OnePlanetMode::CreateScene()
{
    Scene * s = GetScene();
    if ( !rootNode )
        rootNode = s->CreateChild( "OnePlanetMode" );

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "Scenes/OnePlanet.xml" );
    if ( !f )
        return;
    const bool loadedOk = rootNode->LoadXML( f->GetRoot() );
    Node * prefabRoot = rootNode->GetChild( "Root", true );

    Node * camNode = s->GetChild( "MainCamera", true );
    if ( !camNode )
        URHO3D_LOGERROR( "Camera not found" );
    camNode->SetParent( prefabRoot );

    CameraOrb2 * camCtrl = camNode->CreateComponent<CameraOrb2>();

    createObjects();
}

void OnePlanetMode::CreateUI()
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "UI/ToWorkshopPanel.xml" );
    if ( !f )
        return;

    UI * ui = GetSubsystem<UI>();
    UIElement * uiRoot = ui->GetRoot();

    SharedPtr<UIElement> panel = ui->LoadLayout( f );
    uiRoot->AddChild( panel );

    panel->SetPosition( 0, 0 );
    panel->SetAlignment( HA_RIGHT, VA_TOP );
    //panel->SetMinSize( 96, 128 );
    //panel->SetVisible( true );

    UIElement * tryBtn = panel->GetChild( "ToWorkshop", true );
    if ( tryBtn )
        SubscribeToEvent( tryBtn, E_RELEASED,
                          URHO3D_HANDLER( OnePlanetMode, HandleToWorkshop ) );
}





void OnePlanetMode::SubscribeToEvents()
{
    SubscribeToEvent( E_UPDATE,           URHO3D_HANDLER( OnePlanetMode, HandleUpdate ) );
    SubscribeToEvent( E_POSTRENDERUPDATE, URHO3D_HANDLER( OnePlanetMode, HandlePostRenderUpdate ) );

    SubscribeToEvent(E_PHYSICSPRESTEP,   URHO3D_HANDLER( OnePlanetMode, HandlePhysicsPreStep) );
    SubscribeToEvent(E_POSTUPDATE,       URHO3D_HANDLER( OnePlanetMode, HandlePostUpdate) );

    SubscribeToEvent( E_MOUSEBUTTONDOWN, URHO3D_HANDLER( OnePlanetMode, HandleMouseDown ) );
    SubscribeToEvent( E_MOUSEBUTTONUP,   URHO3D_HANDLER( OnePlanetMode, HandleMouseUp ) );
    SubscribeToEvent( E_MOUSEMOVE,       URHO3D_HANDLER( OnePlanetMode, HandleMouseMove ) );
    SubscribeToEvent( E_KEYDOWN,         URHO3D_HANDLER( OnePlanetMode, HandleKeyDown ) );
    SubscribeToEvent( E_KEYUP,           URHO3D_HANDLER( OnePlanetMode, HandleKeyUp ) );
}

void OnePlanetMode::createObjects()
{
    Scene * s = GetScene();
    Node * root = rootNode->GetChild( "Root", true );
    if ( !root )
        return;
    PhysicsWorld * w = s->GetComponent<PhysicsWorld>();
    if ( !w )
        w = s->CreateComponent<PhysicsWorld>();
    DebugRenderer * dr = s->GetComponent<DebugRenderer>();
    if ( !dr )
        dr = s->CreateComponent<DebugRenderer>();

    physicsWorld = SharedPtr<PhysicsWorld>( w );

    Node * surf = root->CreateChild( "Surface" );
    RigidBody * body = surf->CreateComponent<RigidBody>();
    CollisionShape * cs = surf->CreateComponent<CollisionShape>();

    ResourceCache * c = GetSubsystem<ResourceCache>();
    Model * model = c->GetResource<Model>( "Models/Surface.mdl" );
    cs->SetTriangleMesh( model );

    createDesign();
}

void OnePlanetMode::createDesign()
{
    Node * root = rootNode->GetChild( "Root", true );
    if ( !root )
        return;

    LevelManager * lm = GetSubsystem<LevelManager>();
    if ( !lm )
        URHO3D_LOGERROR( "Can\'t get LevelManager" );

    Design d = lm->design();
    Assembly::create( root, d );
}


bool OnePlanetMode::select()
{
    UI * ui = GetSubsystem<UI>();
    IntVector2 pos = ui->GetCursorPosition();
    // Check the cursor is visible and there is no UI element in front of the cursor
    //if ( !ui->GetCursor()->IsVisible() || ui->GetElementAt(pos, true))
    //    return false;

    Graphics * graphics = GetSubsystem<Graphics>();
    Node   * camNode = rootNode->GetChild( "MainCamera", true );
    Camera * camera  = camNode->GetComponent<Camera>();
    Ray cameraRay = camera->GetScreenRay((float)pos.x_ / graphics->GetWidth(), (float)pos.y_ / graphics->GetHeight());

    // Pick only geometry objects, not eg. zones or lights, only get the first (closest) hit
    PODVector<RayQueryResult> results;
    const float maxDistance = 300.0f;
    RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY );
    Scene * s = GetScene();
    s->GetComponent<Octree>()->RaycastSingle( query );
    const size_t qty = results.Size();
    if ( !qty )
    {
        return false;
    }

    RayQueryResult& result = results[0];
    //hitPos = result.position_;
    Node * n = result.node_;
    std::cout << "node name: " << n->GetName().CString() << std::endl;
    if ( !n )
    {
        return false;
    }

    const Vector<SharedPtr<Component> > & comps = n->GetComponents();
    const size_t compsQty = comps.Size();
    Block * b = nullptr;
    for ( size_t i=0; i<compsQty; i++ )
    {
        b = comps[i]->Cast<Block>();
        if ( b )
            break;
    }
    if ( !b )
    {
        return false;
    }


    return true;
}







void OnePlanetMode::HandleUpdate( StringHash t, VariantMap & e )
{
    if ( physicsWorld )
    {
        const Variant & v = e[Update::P_TIMESTEP];
        const float dt = v.GetFloat();
        physicsWorld->Update( dt );
    }
}

void OnePlanetMode::HandlePostRenderUpdate( StringHash t, VariantMap & e )
{
    if ( physicsWorld )
        physicsWorld->DrawDebugGeometry( true );
}

void OnePlanetMode::HandlePhysicsPreStep( StringHash t, VariantMap & e )
{
    const Variant & v = e[Update::P_TIMESTEP];
    const float dt = v.GetFloat();
}

void OnePlanetMode::HandlePostUpdate( StringHash t, VariantMap & e )
{
}

void OnePlanetMode::HandleMouseDown( StringHash t, VariantMap & e )
{
    // Here need to filter out events over UI.
    UI * ui = GetSubsystem<UI>();
    const IntVector2 v = ui->GetCursorPosition();
    /// Return UI element at global screen coordinates. By default returns only input-enabled elements.
    UIElement * w = ui->GetElementAt( v, false );
    if ( w )
        return;


    const int b = e[MouseButtonDown::P_BUTTON].GetInt();
    if ( b == SDL_BUTTON_LEFT )
    {
        // Prepare to select.
    }
    else if ( b == SDL_BUTTON_RIGHT )
    {
        // Context menu???
    }
}

void OnePlanetMode::HandleMouseUp( StringHash t, VariantMap & e )
{
    // Here need to filter out events over UI.
    UI * ui = GetSubsystem<UI>();
    const IntVector2 v = ui->GetCursorPosition();
    /// Return UI element at global screen coordinates. By default returns only input-enabled elements.
    UIElement * w = ui->GetElementAt( v, false );
    if ( w )
        return;


    const int b = e[MouseButtonUp::P_BUTTON].GetInt();
    if ( b == SDL_BUTTON_LEFT )
    {
        // Prepare to select.
        select();
    }
    else if ( b == SDL_BUTTON_RIGHT )
    {
        // Context menu???
    }
}

void OnePlanetMode::HandleMouseMove( StringHash t, VariantMap & e )
{
    mousePrevX = mouseX;
    mousePrevY = mouseY;
    mouseX = e[MouseMove::P_X].GetInt();
    mouseY = e[MouseMove::P_Y].GetInt();
}

void OnePlanetMode::HandleKeyDown( StringHash t, VariantMap & e )
{
    const int key = e[KeyDown::P_KEY].GetInt();
    if ( key == KEY_ESCAPE )
    {
        // Need to show game menu here.
    }

}

void OnePlanetMode::HandleKeyUp( StringHash t, VariantMap & e )
{
    // Do nothing.
}

void OnePlanetMode::HandleToWorkshop( StringHash t, VariantMap & e )
{
    VariantMap& eData = GetEventDataMap();
    eData["Name"] = "Workshop";
    SendEvent(MyEvents::E_SET_LEVEL, eData);
}





}


