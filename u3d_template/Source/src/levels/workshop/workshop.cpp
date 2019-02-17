
#include "workshop.h"
#include "Urho3D/Core/CoreEvents.h"
#include "Urho3D/Input/InputEvents.h"
#include "Urho3D/UI/UIEvents.h"

#include "Global.h"
#include "MyEvents.h"
#include "Audio/AudioManagerDefs.h"
#include "Messages/Achievements.h"
#include "block.h"

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
    // Disable achievement showing for this level
    GetSubsystem<Achievements>()->SetShowAchievements(true);

    if (data_.Contains("Message")) {
        //SharedPtr<Urho3D::MessageBox> messageBox(new Urho3D::MessageBox(context_, data_["Message"].GetString(), "Oh crap!"));
        VariantMap data = GetEventDataMap();
        data["Title"] = "Error!";
        data["Message"] = data_["Message"].GetString();
        SendEvent("ShowAlertMessage", data);
    }
    BaseLevel::Init();

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
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "prefabs/workshop.xml" );
    if ( !f )
        return;
    //scene_->LoadXML( f-> );
    rootNode = scene_->GetChild( "Workshop" );
}

void Workshop::CreateUI()
{
    createSectionsUi();
    createBlocksUi(0);
    createModeUi();
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
    std::vector<CategoryDesc> & cats = techTree.getPanelContent();
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
    std::vector<CategoryDesc> & cats = techTree.getPanelContent();
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


    const std::vector<PartDesc> & blockDescs = techTree.getPartDescs();
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
    _uiRoot = GetSubsystem<UI>()->GetRoot();

    _panelMode = _uiRoot->CreateChild<Window>();

    _panelMode->SetStyleAuto();
    _panelMode->SetAlignment( HA_RIGHT, VA_BOTTOM );
    const int w = _uiRoot->GetWidth()/3;
    const int h = _uiRoot->GetHeight()/10;
    _panelMode->SetSize( w, h );
    _panelMode->SetMinSize( w, h );
    _panelMode->SetLayout( LM_HORIZONTAL );
    _panelMode->SetLayoutBorder( IntRect( 5, 5, 5, 5 ) );


    Text * t = _panelMode->CreateChild<Text>();

    t->SetStyleAuto();
    t->SetEditable( false );
    _panelMode->AddChild( t );

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

}

void Workshop::cameraPlane( Vector3 & x, Vector3 & y, Vector3 & n )
{
    Camera * c = _cameras[0];
    Vector3 a( 0.0, 0.0, -1.0 );
    const Quaternion q = c->GetNode()->GetRotation();
    a = q * a;
    // Check if "y" abs value is > 0.707 or not.
    const float TH = 0.707f;
    const bool vert = ( std::abs( a.y ) <= TH );
    if ( vert )
    {
        x = Vector3( 1.0, 0.0, 0.0 );
        x = q * x;

        y = Vector3( 0.0, 1.0, 0.0 );
        n = x.CrossProduct( y );
        return;
    }
    x = Ogre::Vector3( 1.0, 0.0, 0.0 );
    x = q * x;
    x = Ogre::Vector3( x.x, 0.0, x.z );
    x.normalise();

    n =  Ogre::Vector3( 0.0, 1.0, 0.0 );
    y = n.crossProduct( x );
}

void Workshop::mouseIntersection( Vector3 & at, const Vector3 & origin )
{

}

void Workshop::drag()
{

}

void Workshop::dragStart()
{

}

void Workshop::dragStop()
{

}

void Workshop::rotate()
{

}

void Workshop::rotateStart()
{

}

void Workshop::rotateStop()
{

}


void Workshop::HandlePhysicsPreStep( StringHash t, VariantMap & e )
{

}

void Workshop::HandlePostUpdate( StringHash t, VariantMap & e )
{

}

void Workshop::HandleMouseDown( StringHash t, VariantMap & e )
{
    const int b = e[MouseButtonDown::P_BUTTON].GetInt();
    if ( b == SDL_BUTTON_LMASK )
    {
        // Prepare to select.
    }
    else if ( b == SDL_BUTTON_RMASK )
    {
        // Context menu???
    }
}

void Workshop::HandleMouseUp( StringHash t, VariantMap & e )
{
    const int b = e[MouseButtonUp::P_BUTTON].GetInt();
    if ( mode != None )
    {
        if ( b == SDL_BUTTON_LMASK )
        {
            // Prepare to select.
        }
        else if ( b == SDL_BUTTON_RMASK )
        {
            // Context menu???
        }
    }
}

void Workshop::HandleMouseMove( StringHash t, VariantMap & e )
{
    mouseX = e[MouseMove::P_X].GetInt();
    mouseY = e[MouseMove::P_Y].GetInt();
    if ( mode == Drag )
        ;
    else if ( mode == Rotate )
        ;
}

void Workshop::HandleKeyDown( StringHash t, VariantMap & e )
{
    const int key = e[KeyDown::P_KEY].GetInt();
    if ( selectedBlock )
    {
        if ( key == KEY_G )
            mode = Drag;
        else if ( key == KEY_R )
            mode = Rotate;
        else if (( key == KEY_ESCAPE ) && (mode != None))
            mode = None;
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
    // Create a part of this name.
    Object * o = context_->CreateObject( StringHash(typeName) );
    if ( !o )
        return;

    Block * b = o->Cast<Block>();
    b->setParent( rootNode );
    mode = Drag;
}



}


