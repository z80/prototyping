
#include "workshop.h"
#include "Global.h"
#include "MyEvents.h"
#include "Audio/AudioManagerDefs.h"
#include "Messages/Achievements.h"
#include "block.h"

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
      mode( None )
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

//    data["Title"] = "Hey!";
//    data["Message"] = "Seems like everything is ok!";
//    SendEvent("ShowAlertMessage", data);
}

void Workshop::CreateScene()
{
    //scene_->LoadXML( "Data/prefabs/workshop.xml" );
    rootNode = scene_->GetChild( "Workshop" );
}

void Workshop::CreateUI()
{
    createSectionsUi();
    //createBlocksUi(0);
    //createModeUi();
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
    _panelMain->BringToFront();

    return;

     _panelGroups = _panelMain->CreateChild<Window>();

    _panelMain->SetStyleAuto();
    _panelGroups->SetAlignment( HA_LEFT, VA_TOP );
    //_panelGroups->SetSize( 128, _uiRoot->GetHeight() );
    _panelGroups->SetSize( 256, 256 );
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
        SubscribeToEvent( b, E_RELEASED,
            [&]( StringHash eventType, VariantMap & eventData)
            {
                const int ind = i;
                VariantMap & data = GetEventDataMap();
                data[ "index" ] = ind;
                SendEvent( E_CATEGORY_CLICKED, data );
            });
    }
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
        SubscribeToEvent( b, E_RELEASED,
            [&]( StringHash eventType, VariantMap & eventData )
            {
                const String name = pd.name;
                VariantMap & data = GetEventDataMap();
                data[ "name" ] = name;
                SendEvent( E_CATEGORY_CLICKED, data );
            });

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
    SubscribeToEvent( E_CATEGORY_CLICKED,     URHO3D_HANDLER( Workshop, HandlePanelGroupClicked ) );
    SubscribeToEvent( E_CREATE_BLOCK_CLICKED, URHO3D_HANDLER( Workshop, HandlePanelBlockClicked ) );
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

void Workshop::HandlePanelGroupClicked( StringHash eventType, VariantMap & eventData )
{
    const Variant v = eventData[ "index" ];
    const int ind = v.GetInt();
    createBlocksUi( ind );
}

void Workshop::HandlePanelBlockClicked( StringHash eventType, VariantMap & eventData )
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


