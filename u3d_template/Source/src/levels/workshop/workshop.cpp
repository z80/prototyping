
#include "workshop.h"
#include "Global.h"
#include "MyEvents.h"
#include "Audio/AudioManagerDefs.h"
#include "Messages/Achievements.h"


namespace Osp
{

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

    _window = new Window( context_ );
    _uiRoot->AddChild( _window );
    _window->SetAlignment( HA_LEFT, VA_CENTER );
    _window->SetSize( 64, _uiRoot->GetHeight() );
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


}



