
#include "workshop.h"
#include "Global.h"
#include "MyEvents.h"
#include "Audio/AudioManagerDefs.h"
#include "Messages/Achievements.h"


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
    : BaseLevel(context)
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
    scene_->LoadXML( "Data/prefabs/workshop.xml" );
}

void Workshop::CreateUI()
{
    _uiRoot = GetSubsystem<UI>()->GetRoot();
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * style = cache->GetResource<XMLFile>( "UI/DefaultStyle.xml" );
    _uiRoot->SetDefaultStyle( style );

    Input* input = GetSubsystem<Input>();
    if ( !input->IsMouseVisible() )
        input->SetMouseVisible(true);

    Localization * localization = GetSubsystem<Localization>();

    _panelMain = new Window( context_ );
    _uiRoot->AddChild( _panelMain );
    _panelMain->SetAlignment( HA_LEFT, VA_CENTER );
    _panelMain->SetSize( 128, _uiRoot->GetHeight() );
    _panelMain->SetLayout( LM_HORIZONTAL );
    _panelMain->SetLayoutBorder( IntRect( 5, 5, 5, 5 ) );

    _panelGroups = new Window( context_ );
    _panelMain->AddChild( _panelGroups );
    _panelGroups->SetAlignment( HA_LEFT, VA_CENTER );
    _panelGroups->SetSize( 64, _uiRoot->GetHeight() );
    _panelGroups->SetLayout( LM_VERTICAL );
    _panelGroups->SetLayoutBorder( IntRect( 5, 5, 5, 5 ) );

    // Create categories.
    std::vector<CategoryDesc> & cats = techTree->getPanelContent();
    const size_t qty = cats.size();
    for ( size_t i=0; i<qty; i++ )
    {
        const CategoryDesc & c = cats[i];

        Button * b = _panelGroups->CreateChild<Button>();
        XMLFile * style = cache->GetResource<XMLFile>( c.icon );
        b->SetDefaultStyle( style );
        SubscribeToEvent( b, E_RELEASED,
            [&]( StringHash eventType, VariantMap & eventData)
            {
                 VariantMap & data = GetEventDataMap();
                 data[ "index" ] = i;
                 SendEvent( E_CATEGORY_CLICKED, data );
            });
    }
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

    _panelBlocks = new Window( context_ );
    _panelMain->AddChild( _panelBlocks );
    _panelBlocks->SetAlignment( HA_LEFT, VA_CENTER );
    _panelBlocks->SetSize( 64, _uiRoot->GetHeight() );
    _panelBlocks->SetLayout( LM_VERTICAL );
    _panelBlocks->SetLayoutBorder( IntRect( 5, 5, 5, 5 ) );


    const CategoryDesc & c = cats[groupInd];
    const size_t typesQty = c.items.size();
    for ( size_t i=0; i<typesQty; i++ )
    {

    }
}

void Workshop::SubscribeToEvents()
{
    SubscribeToEvent( E_CATEGORY_CLICKED,     URHO3D_HANDLER( Workshop, HandlePanelGroupClicked ) );
    SubscribeToEvent( E_CREATE_BLOCK_CLICKED, URHO3D_HANDLER( Workshop, HandlePanelBlockClicked ) )
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

}

void Workshop::HandlePanelBlockClicked( StringHash eventType, VariantMap & eventData )
{

}



}



