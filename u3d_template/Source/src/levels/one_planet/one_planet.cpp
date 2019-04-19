
#include "one_planet.h"
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
#include "game_data.h"
#include "design_manager.h"
#include "block.h"
#include "assembly.h"
#include "camera_orb_2.h"
#include "kepler_mover.h"
#include "planet_test.h"

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


OnePlanet::OnePlanet(Context* context)
    : BaseLevel(context),
      mouseX(0),
      mouseY(0),
      mousePrevX(0),
      mousePrevY(0)
{
}

OnePlanet::~OnePlanet()
{

}

void OnePlanet::Init()
{
    BaseLevel::Init();

    // Disable achievement showing for this level
    GetSubsystem<Achievements>()->SetShowAchievements(true);

    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateUI();

    // Subscribing to events.
    SubscribeToEvents();

//    data["Title"] = "Hey!";
//    data["Message"] = "Seems like everything is ok!";
//    SendEvent("ShowAlertMessage", data);
}

void OnePlanet::Finit()
{
    if ( rootNode )
        rootNode->Remove();
    if ( panel )
        panel->Remove();
}

void OnePlanet::CreateScene()
{
    Input * inp = GetSubsystem<Input>();
    inp->SetMouseVisible( true );

    ControllerInput * controllerInput = GetSubsystem<ControllerInput>();
    Vector<int> controlIndexes = controllerInput->GetControlIndexes();
    InitViewports(controlIndexes);

    /*ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "Prefabs/OnePlanet.xml" );
    if ( !f )
        return;*/

    //const bool loadedOk = scene_->LoadXML( f->GetRoot() );
    //rootNode = scene_->GetChild( "Root", true );
    rootNode = scene_->CreateChild( "Root" );
    //const bool loadedOk = rootNode->LoadXML( f->GetRoot() );

    Node * camNode = _cameras[0];
    camNode->SetParent( rootNode );

    CameraOrb2 * camCtrl = camNode->CreateComponent<CameraOrb2>();
    //camCtrl->updateCamera();

    createObjects();
}

void OnePlanet::CreateUI()
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "UI/ToWorkshopPanel.xml" );
    if ( !f )
        return;

    UI * ui = GetSubsystem<UI>();
    UIElement * uiRoot = ui->GetRoot();

    panel = ui->LoadLayout( f );
    uiRoot->AddChild( panel );

    panel->SetPosition( 0, 0 );
    panel->SetAlignment( HA_RIGHT, VA_TOP );

    UIElement * tryBtn = panel->GetChild( "ToWorkshop", true );
    if ( tryBtn )
        SubscribeToEvent( tryBtn, E_RELEASED,
                          URHO3D_HANDLER( OnePlanet, HandleToWorkshop ) );
}





void OnePlanet::SubscribeToEvents()
{
    SubscribeToEvent( E_UPDATE,           URHO3D_HANDLER( OnePlanet, HandleUpdate ) );
    SubscribeToEvent( E_POSTRENDERUPDATE, URHO3D_HANDLER( OnePlanet, HandlePostRenderUpdate ) );

    SubscribeToEvent(E_PHYSICSPRESTEP,   URHO3D_HANDLER( OnePlanet, HandlePhysicsPreStep) );
    SubscribeToEvent(E_POSTUPDATE,       URHO3D_HANDLER( OnePlanet, HandlePostUpdate) );

    SubscribeToEvent( E_MOUSEBUTTONDOWN, URHO3D_HANDLER( OnePlanet, HandleMouseDown ) );
    SubscribeToEvent( E_MOUSEBUTTONUP,   URHO3D_HANDLER( OnePlanet, HandleMouseUp ) );
    SubscribeToEvent( E_MOUSEMOVE,       URHO3D_HANDLER( OnePlanet, HandleMouseMove ) );
    SubscribeToEvent( E_KEYDOWN,         URHO3D_HANDLER( OnePlanet, HandleKeyDown ) );
    SubscribeToEvent( E_KEYUP,           URHO3D_HANDLER( OnePlanet, HandleKeyUp ) );
}

void OnePlanet::createObjects()
{
    Node * root = rootNode.Get();

    /*PhysicsWorld * w = scene_->GetComponent<PhysicsWorld>();
    if ( !w )
        w = scene_->CreateComponent<PhysicsWorld>();
    DebugRenderer * dr = scene_->GetComponent<DebugRenderer>();
    if ( !dr )
        dr = scene_->CreateComponent<DebugRenderer>();

    physicsWorld = SharedPtr<PhysicsWorld>( w );

    Node * surf = root->CreateChild( "Surface" );
    RigidBody * body = surf->CreateComponent<RigidBody>();
    CollisionShape * s = surf->CreateComponent<CollisionShape>();

    ResourceCache * c = GetSubsystem<ResourceCache>();
    Model * model = c->GetResource<Model>( "Models/Surface.mdl" );
    s->SetTriangleMesh( model );*/

    createKepler();
    createDesign();
}

void OnePlanet::createDesign()
{
    Node * root = scene_->GetChild( "Root", true );
    if ( !root )
        return;

    GameData * gd = scene_->GetOrCreateComponent<GameData>();
    if ( !gd )
        URHO3D_LOGERROR( "Can\'t get GameData" );

    Design d = gd->design;
    Assembly::create( root, d );
}

void OnePlanet::createKepler()
{
    Node * rotCenter = rootNode->CreateChild( "RotationCenter" );
    {
        StaticModel * m = rotCenter->CreateComponent<StaticModel>();
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        m->SetModel( cache->GetResource<Model>( "Models/Sphere.mdl" ) );
        m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
        m->SetCastShadows( true );
        rotCenter->SetPosition( Vector3( 0.0, 3.0, 0.0 ) );
    }
    /*{
        Node * body = rotCenter->CreateChild( "OrbitingBody" );
        StaticModel * m = body->CreateComponent<StaticModel>();
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        m->SetModel( cache->GetResource<Model>( "Models/Sphere.mdl" ) );
        m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
        m->SetCastShadows( true );
        body->SetPosition( Vector3( 0.0, 3.0, 0.0 ) );

        KeplerMover * km = body->CreateComponent<KeplerMover>();
        km->initKepler( 100.0, 5.0, 0.8, 0.0, 0.0, 0.0, 0.0 );
    }*/
    /*{
        Node * body = rotCenter->CreateChild( "OrbitingBodyInit" );
        StaticModel * m = body->CreateComponent<StaticModel>();
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        m->SetModel( cache->GetResource<Model>( "Models/Sphere.mdl" ) );
        m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
        m->SetCastShadows( true );
        body->SetPosition( Vector3( 0.0, 8.0, 0.0 ) );

        KeplerMover * km = body->CreateComponent<KeplerMover>();
        km->GM = 100.0;
        // Hyperbolic.
        //km->launch( Vector3d( 6.0, 0.0, 0.0 ) );
        // This is exactly parabolic.
        //km->launch( Vector3( 5.0, 0.0, 0.0 ) );
        // Elliptic
        km->launch( Vector3( 3.0, 0.0, 0.0 ) );
    }*/

    /*{
        const Vector3 start( 10.0, 10.0, 0.0 );

        Node * body = rotCenter->CreateChild( "OrbitingBodyInit" );
        StaticModel * m = body->CreateComponent<StaticModel>();
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        m->SetModel( cache->GetResource<Model>( "Models/Sphere.mdl" ) );
        m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
        m->SetCastShadows( true );
        body->SetPosition( start );

        KeplerMover * km = body->CreateComponent<KeplerMover>();
        km->GM = 100.0;
        // This is exactly parabolic.
        //km->launch( Vector3( 5.0, 0.0, 0.0 ) );
        km->launch( Vector3d( 0.0, 0.0, -1.0 ) );

        // Reference body
        body = rotCenter->CreateChild( "Reference" );
        m = body->CreateComponent<StaticModel>();
        m->SetModel( cache->GetResource<Model>( "Models/Sphere.mdl" ) );
        m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
        m->SetCastShadows( true );
        body->SetPosition( start );

    }*/

    // Create "PlanetTest" instance.
    {
        Zone * z = rootNode->CreateComponent<Zone>();
        z->SetAmbientColor( Color( 0.8, 0.8, 0.8, 1.0 ) );

        Node * planetNode = rootNode->CreateChild( "PlanetNode" );
        PlanetTest * pt = planetNode->CreateComponent<PlanetTest>();
    }
}


bool OnePlanet::select()
{
    UI * ui = GetSubsystem<UI>();
    IntVector2 pos = ui->GetCursorPosition();
    // Check the cursor is visible and there is no UI element in front of the cursor
    //if ( !ui->GetCursor()->IsVisible() || ui->GetElementAt(pos, true))
    //    return false;

    Graphics * graphics = GetSubsystem<Graphics>();
    Node   * camNode = _cameras[0];
    Camera * camera  = camNode->GetComponent<Camera>();
    Ray cameraRay = camera->GetScreenRay((float)pos.x_ / graphics->GetWidth(), (float)pos.y_ / graphics->GetHeight());

    // Pick only geometry objects, not eg. zones or lights, only get the first (closest) hit
    PODVector<RayQueryResult> results;
    const float maxDistance = 300.0f;
    RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY );
    scene_->GetComponent<Octree>()->RaycastSingle( query );
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







void OnePlanet::HandleUpdate( StringHash t, VariantMap & e )
{
    if ( physicsWorld )
    {
        const Variant & v = e[Update::P_TIMESTEP];
        const float dt = v.GetFloat();
        physicsWorld->Update( dt );
    }
}

void OnePlanet::HandlePostRenderUpdate( StringHash t, VariantMap & e )
{
    if ( physicsWorld )
        physicsWorld->DrawDebugGeometry( true );
}

void OnePlanet::HandlePhysicsPreStep( StringHash t, VariantMap & e )
{
    const Variant & v = e[Update::P_TIMESTEP];
    const float dt = v.GetFloat();
}

void OnePlanet::HandlePostUpdate( StringHash t, VariantMap & e )
{
}

void OnePlanet::HandleMouseDown( StringHash t, VariantMap & e )
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

void OnePlanet::HandleMouseUp( StringHash t, VariantMap & e )
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

void OnePlanet::HandleMouseMove( StringHash t, VariantMap & e )
{
    mousePrevX = mouseX;
    mousePrevY = mouseY;
    mouseX = e[MouseMove::P_X].GetInt();
    mouseY = e[MouseMove::P_Y].GetInt();
}

void OnePlanet::HandleKeyDown( StringHash t, VariantMap & e )
{
    const int key = e[KeyDown::P_KEY].GetInt();
    if ( key == KEY_ESCAPE )
    {
        // Need to show game menu here.
    }

}

void OnePlanet::HandleKeyUp( StringHash t, VariantMap & e )
{
    // Do nothing.
}

void OnePlanet::HandleToWorkshop( StringHash t, VariantMap & e )
{
    VariantMap& eData = GetEventDataMap();
    eData["Name"] = "Workshop";
    SendEvent(MyEvents::E_SET_LEVEL, eData);
}





}


