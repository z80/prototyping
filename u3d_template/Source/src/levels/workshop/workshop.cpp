
#include "workshop.h"
#include "Urho3D/Core/CoreEvents.h"
#include "Urho3D/Input/InputEvents.h"
#include "Urho3D/UI/UIEvents.h"

#include "Global.h"
#include "MyEvents.h"
#include "Audio/AudioManagerDefs.h"
#include "Messages/Achievements.h"
#include "Input/ControllerInput.h"

#include "name_generator.h"
#include "block.h"
#include "camera_orb_2.h"

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


Workshop::Workshop(Context* context)
    : BaseLevel(context),
      mode( None ),
      mouseX(0),
      mouseY(0)
{
}

Workshop::~Workshop()
{
}

void Workshop::Init()
{
    if (!scene_)
    {
        // There is no scene, get back to the main menu
        VariantMap& eventData = GetEventDataMap();
        eventData["Name"] = "MainMenu";
        SendEvent(MyEvents::E_SET_LEVEL, eventData);

        return;
    }

    BaseLevel::Init();

    techTree = GetSubsystem<TechTree>();

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

void Workshop::CreateScene()
{
    Input * inp = GetSubsystem<Input>();
    inp->SetMouseVisible( true );

    ControllerInput * controllerInput = GetSubsystem<ControllerInput>();
    Vector<int> controlIndexes = controllerInput->GetControlIndexes();
    InitViewports(controlIndexes);

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "Scenes/Workshop.xml" );
    if ( !f )
        return;
    const bool loadedOk = scene_->LoadXML( f->GetRoot() );
    rootNode = scene_->GetChild( "Workshop", true );
    //rootNode = scene_->GetChild( StringHash( "Workshop" ), true );

    Node * camNode = _cameras[0];
    camNode->SetParent( rootNode );

    CameraOrb2 * camCtrl = camNode->CreateComponent<CameraOrb2>();
    //camCtrl->updateCamera();
}

void Workshop::CreateUI()
{
    //createSectionsUi();
    //createBlocksUi(0);
    createModeUi();
    createAuxilaryPanel();
}

void Workshop::createSectionsUi()
{
    _uiRoot = GetSubsystem<UI>()->GetRoot();
    //_uiRoot->SetLayout( LM_FREE );

    Input* input = GetSubsystem<Input>();
    if ( !input->IsMouseVisible() )
        input->SetMouseVisible(true);

    Localization * localization = GetSubsystem<Localization>();

    _panelMain = _uiRoot->CreateChild<Window>();

    _panelMain->SetStyleAuto();
    _panelMain->SetAlignment(HA_LEFT, VA_CENTER);
    _panelMain->SetSize(400, 500);
    _panelMain->SetMinSize( 400, 500 );
    _panelMain->SetLayout( LM_HORIZONTAL );
    _panelMain->BringToFront();

     _panelGroups = _panelMain->CreateChild<Window>();

    _panelGroups->SetStyleAuto();
    _panelGroups->SetAlignment( HA_LEFT, VA_TOP );
    //_panelGroups->SetSize( 128, _uiRoot->GetHeight() );
    _panelGroups->SetSize( 256, 256 );
    _panelGroups->SetMinSize( 400, 500 );
    _panelGroups->SetLayout( LM_VERTICAL );
    _panelGroups->SetLayoutBorder( IntRect( 5, 5, 5, 5 ) );


    // Create categories.
    std::vector<CategoryDesc> & cats = techTree->getPanelContent();
    const size_t qty = cats.size();
    for ( size_t i=0; i<qty; i++ )
    {
        const CategoryDesc & c = cats[i];

        Button * b = _panelGroups->CreateChild<Button>();
        b->SetStyleAuto();
        b->SetMaxSize( 48, 48 );

        SubscribeToEvent( b, E_RELEASED,
            std::bind( &Workshop::HandlePanelGroupClicked,
                       this, i ) );
    }

    return;

}

void Workshop::createBlocksUi( int groupInd )
{
    std::vector<CategoryDesc> & cats = techTree->getPanelContent();
    const size_t qty = cats.size();
    if ( groupInd >= qty )
        return;

    // This is supposed to destroy the panel with all buttons.
    if( _panelBlocks )
        _panelMain->RemoveChild( _panelBlocks );

     _panelBlocks = _panelMain->CreateChild<Window>();

    _panelBlocks->SetStyleAuto();
    _panelBlocks->SetAlignment( HA_LEFT, VA_CENTER );
    //_panelBlocks->SetSize( 64, _uiRoot->GetHeight() );
    _panelBlocks->SetSize( 64, 64 );
    _panelBlocks->SetMinSize( 64, 64 );
    _panelBlocks->SetLayout( LM_VERTICAL );
    _panelBlocks->SetLayoutBorder( IntRect( 5, 5, 5, 5 ) );


    const std::vector<PartDesc> & blockDescs = techTree->getPartDescs();
    const CategoryDesc & c = cats[groupInd];
    const size_t typesQty = c.items.size();
    for ( size_t i=0; i<typesQty; i++ )
    {
        const int ind = c.items[i];
        const PartDesc & pd = blockDescs[ind];

        Button * b = _panelBlocks->CreateChild<Button>();
        b->SetStyleAuto();
        b->SetMaxSize( 48, 48 );
        SubscribeToEvent( b, E_RELEASED,
                          std::bind( &Workshop::HandlePanelBlockClicked,
                                     this, pd.name ) );

    }
}

void Workshop::createModeUi()
{
    Graphics * graphics = GetSubsystem<Graphics>();
    const int w = graphics->GetWidth();
    const int h = graphics->GetHeight();

    _uiRoot = GetSubsystem<UI>()->GetRoot();

    Text * t = _uiRoot->CreateChild<Text>();
    t->SetStyleAuto();
    t->SetEditable( false );
    t->SetPosition( w / 2, h * 8 / 10 );
    t->SetText( "Hello!" );

    _modeText = t;
}

void Workshop::SubscribeToEvents()
{
    SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER( Workshop, HandlePhysicsPreStep) );
    SubscribeToEvent(E_POSTUPDATE,     URHO3D_HANDLER( Workshop, HandlePostUpdate) );

    SubscribeToEvent( E_MOUSEBUTTONDOWN, URHO3D_HANDLER( Workshop, HandleMouseDown ) );
    SubscribeToEvent( E_MOUSEBUTTONUP,   URHO3D_HANDLER( Workshop, HandleMouseUp ) );
    SubscribeToEvent( E_MOUSEMOVE,       URHO3D_HANDLER( Workshop, HandleMouseMove ) );
    SubscribeToEvent( E_KEYDOWN,         URHO3D_HANDLER( Workshop, HandleKeyDown ) );
    SubscribeToEvent( E_KEYUP,           URHO3D_HANDLER( Workshop, HandleKeyUp ) );

    SubscribeToEvent( E_CATEGORY_CLICKED,     URHO3D_HANDLER( Workshop, HandlePanelGroupSelected ) );
    SubscribeToEvent( E_CREATE_BLOCK_CLICKED, URHO3D_HANDLER( Workshop, HandlePanelBlockSelected ) );
}

Button* Workshop::CreateButton(const String& text, int width, IntVector2 position)
{
    auto* cache = GetSubsystem<ResourceCache>();
    auto* font = cache->GetResource<Font>(APPLICATION_FONT);

    auto* button = GetSubsystem<UI>()->GetRoot()->CreateChild<Button>();
    button->SetStyleAuto();
    button->SetFixedWidth(width);
    button->SetFixedHeight(30);
    button->SetPosition(position);

    auto* buttonText = button->CreateChild<Text>();
    buttonText->SetFont(font, 12);
    buttonText->SetAlignment(HA_CENTER, VA_CENTER);
    buttonText->SetText(text);

    return button;
}

bool Workshop::select()
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
        selectedBlock.Reset();
        hintDefault();
        return false;
    }

    RayQueryResult& result = results[0];
    //hitPos = result.position_;
    Node * n = result.node_;
    std::cout << "node name: " << n->GetName().CString() << std::endl;
    if ( !n )
    {
        selectedBlock.Reset();
        hintDefault();
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
        selectedBlock.Reset();
        hintDefault();
        return false;
    }

    selectedBlock = SharedPtr<Block>( b );
    hintSelected();

    return true;
}

void Workshop::cameraPlane( Vector3 & x, Vector3 & y, Vector3 & n )
{
    Node * camNode = _cameras[0];
    Camera * cam = camNode->GetComponent<Camera>();
    Vector3 a( 0.0, 0.0, 1.0 );
    const Quaternion q = camNode->GetRotation();
    a = q * a;
    // Check if "y" abs value is > 0.707 or not.
    const float TH = 0.707f;
    const bool vert = ( std::abs( a.y_ ) <= TH );
    if ( vert )
    {
        x = Vector3( 1.0, 0.0, 0.0 );
        x = q * x;

        y = Vector3( 0.0, 1.0, 0.0 );
        n = x.CrossProduct( y );
        return;
    }
    x = Vector3( 1.0, 0.0, 0.0 );
    x = q * x;
    x = Vector3( x.x_, 0.0, x.z_ );
    x.Normalize();

    n = Vector3( 0.0, 1.0, 0.0 );
    y = n.CrossProduct( x );
}

void Workshop::mouseIntersection( Vector3 & at, const Vector3 & origin )
{
    const float DEG2RAD = 3.1415f / 180.0f;

    UI * ui = GetSubsystem<UI>();
    const IntVector2 pos = ui->GetCursorPosition();

    Graphics * graphics = GetSubsystem<Graphics>();

    Node * camNode = _cameras[0];
    Camera * cam = camNode->GetComponent<Camera>();
    const int w = graphics->GetWidth();
    const int h = graphics->GetHeight();

    /*Ray cameraRay = cam->GetScreenRay((float)pos.x_ / w, (float)pos.y_ / h);

    const float fovX  = cam->GetFov() * DEG2RAD / 2.0f;
    const float ratio = cam->GetAspectRatio();
    const float fovY  = fovX / ratio;
    const float tx = std::tan( fovX );
    const float ty = std::tan( fovY );
    const float ax = float(2*pos.x_ - w) / float( w );
    const float ay = float(h - 2*pos.y_) / float( h );
    Vector3 a( ax*tx, ay*ty, 1.0 );*/

    const Matrix4 projInverse = cam->GetProjection().Inverse();

    float x_ = float(pos.x_) / float(w);
    float y_ = float(pos.y_) / float(h);
    // The parameters range from 0.0 to 1.0. Expand to normalized device coordinates (-1.0 to 1.0) & flip Y axis
    x_ = 2.0f * x_ - 1.0f;
    y_ = 1.0f - 2.0f * y_;
    Vector3 near(x_, y_, 0.0f);
    Vector3 far(x_, y_, 1.0f);

    const Vector3 origin_    = projInverse * near;
    const Vector3 direction_ = ((projInverse * far) - origin_).Normalized();

    Vector3    a = Vector3::ZERO;
    Vector3    rel_r = Vector3::ZERO;
    Quaternion rel_q;
    {
        Node * rootNode = scene_->GetChild( "Workshop" );
        ItemBase::relativePose( camNode, rootNode, rel_r, rel_q );

        a = rel_q*direction_;
        rel_r = rel_r + rel_q*origin_;
    }

    Vector3 x, y, n;
    cameraPlane( x, y, n );
    // (a*t + r_rel - origin)*n = 0
    // (a,n)*t = (origin-r_rel,n)
    float t_den = a.DotProduct( n );
    if ( std::abs( t_den ) < 0.001 )
        at = Vector3( 0.0, 0.0, 0.0 );
    const float t = (origin-rel_r).DotProduct( n ) / t_den;
    at = a*t + rel_r;
}

void Workshop::hintDefault()
{
    _modeText->SetText( "Create blocks using side panel.\n"
                        "Select existing blocks with LMB.\n"
                        "Use RMB for context menu." );
}

void Workshop::hintSelected()
{
    _modeText->SetText( "Use \"g\" to drag object and \n"
                        "\"r\" to rotate it. Click LMB "
                        "to finish modification." );
}

void Workshop::hintDragged()
{
    _modeText->SetText( "Object is being dragged. Press "
                        "LMB to finish ot ESC to cancel." );
}

void Workshop::hintRotated()
{
    _modeText->SetText( "Object is being rotated. Press "
                        "LMB to finish ot ESC to cancel." );
}

void Workshop::windowBlockParams()
{
    Graphics * graphics = GetSubsystem<Graphics>();
    const int wi = graphics->GetWidth();
    const int he = graphics->GetHeight();

    _uiRoot = GetSubsystem<UI>()->GetRoot();

    Window * w = _uiRoot->CreateChild<Window>();
    w->SetStyleAuto();
    w->SetMinSize( 150, 300 );
    w->SetMaxSize( 150, 300 );
    w->SetLayout( LM_VERTICAL );
    w->SetAlignment( HA_RIGHT, VA_TOP );
    w->SetLayoutBorder( IntRect( 5, 5, 5, 5 ) );

    LineEdit * posX = w->CreateChild<LineEdit>();
    posX->SetName( "posX" );
    LineEdit * posY = w->CreateChild<LineEdit>();
    posY->SetName( "posY" );
    LineEdit * posZ = w->CreateChild<LineEdit>();
    posZ->SetName( "posZ" );
}

void Workshop::createAuxilaryPanel()
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "UI/AuxPanel.xml" );
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

    UIElement * save = panel->GetChild( "SaveDesign", true );
    SubscribeToEvent( save, E_RELEASED,
                       URHO3D_HANDLER( Workshop, HandleSaveDesignDialog ) );
    UIElement * load = panel->GetChild( "LoadDesign", true );
    SubscribeToEvent( load, E_RELEASED,
                       URHO3D_HANDLER( Workshop, HandleLoadDesignDialog ) );
}

void Workshop::showPivots( bool en )
{
    Node * rootNode = scene_->GetChild( "Workshop" );
    PODVector<Component *> comps;
    rootNode->GetComponents( comps, StringHash( "Block" ), true );
    const size_t qty = comps.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        Block * b = c->Cast<Block>();
        if ( !b )
            continue;
        b->setPivotsVisible( en );
    }
}

void Workshop::drag()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }

    const Vector3 origin = selectedBlock->relR();
    Vector3 at;
    mouseIntersection( at, origin );

    selectedBlock->setR( at );
}

void Workshop::dragStart()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }
    hintDragged();
    showPivots( true );

    selectedBlock->detach();
    mode = Drag;
}

void Workshop::dragStop()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }
    hintSelected();
    showPivots( false );

    selectedBlock->tryAttach();

    mode = None;
}

void Workshop::rotate()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }


}

void Workshop::rotateStart()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }
    hintRotated();
    showPivots( true );

    selectedBlock->detach();

    mode = Rotate;
}

void Workshop::rotateStop()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }
    hintSelected();
    showPivots( false );

    selectedBlock->tryAttach();
    mode = None;
}


void Workshop::HandlePhysicsPreStep( StringHash t, VariantMap & e )
{

}

void Workshop::HandlePostUpdate( StringHash t, VariantMap & e )
{

    if ( mode == Drag )
        drag();
    else if ( mode == Rotate )
        rotate();
}

void Workshop::HandleMouseDown( StringHash t, VariantMap & e )
{
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

void Workshop::HandleMouseUp( StringHash t, VariantMap & e )
{
    const int b = e[MouseButtonUp::P_BUTTON].GetInt();
    if ( b == SDL_BUTTON_LEFT )
    {
        // Prepare to select.
        if ( mode == None )
            select();
        else if ( mode == Drag )
            dragStop();
        else if ( mode == Rotate )
            rotateStop();
    }
    else if ( b == SDL_BUTTON_RIGHT )
    {
        // Context menu???
    }
}

void Workshop::HandleMouseMove( StringHash t, VariantMap & e )
{
    mouseX = e[MouseMove::P_X].GetInt();
    mouseY = e[MouseMove::P_Y].GetInt();
    if ( mode == Drag )
        drag();
    else if ( mode == Rotate )
        rotate();
}

void Workshop::HandleKeyDown( StringHash t, VariantMap & e )
{
    const int key = e[KeyDown::P_KEY].GetInt();
    if ( selectedBlock )
    {
        if ( key == KEY_G )
            dragStart();
        else if ( key == KEY_R )
            rotateStart();
        else if ( key == KEY_ESCAPE )
        {
            if ( mode == Drag )
                dragStop();
            else if ( mode == Rotate )
                rotateStop();
            else if ( mode == None )
            {
                selectedBlock = nullptr;
                hintDefault();
            }
        }
    }
    if ( key == KEY_ESCAPE )
    {
        // Need to show game menu here.
    }

}

void Workshop::HandleKeyUp( StringHash t, VariantMap & e )
{
    // Do nothing.
}


void Workshop::HandlePanelGroupClicked( int ind )
{
    VariantMap & data = GetEventDataMap();
    data[ "index" ] = ind;
    SendEvent( E_CATEGORY_CLICKED, data );
}

void Workshop::HandlePanelBlockClicked( const String name )
{
    VariantMap & data = GetEventDataMap();
    data[ "name" ] = name;
    SendEvent( E_CREATE_BLOCK_CLICKED, data );
}

void Workshop::HandlePanelGroupSelected( StringHash eventType, VariantMap & eventData )
{
    const Variant v = eventData[ "index" ];
    const int ind = v.GetInt();
    createBlocksUi( ind );
}

void Workshop::HandlePanelBlockSelected( StringHash eventType, VariantMap & eventData )
{
    const Variant v = eventData[ "name" ];
    const String typeName = v.GetString();
    rootNode = scene_->GetChild( "Workshop" );
    // Create a part of this name.
    Node * n = rootNode->CreateChild( NameGenerator::Next( typeName ) );
    std::cout << "new block node name: " << n->GetName().CString() << std::endl;
    Object * o = n->CreateComponent( StringHash( typeName ) );
    if ( !o )
        return;

    Block * b = o->Cast<Block>();
    b->createContent();

    selectedBlock = SharedPtr<Block>( b );

    b->setParent( rootNode );
    dragStart();
}

void Workshop::HandleSaveDesignDialog( StringHash eventType, VariantMap & eventData )
{

}

void Workshop::HandleLoadDesignDialog( StringHash eventType, VariantMap & eventData )
{

}




}


