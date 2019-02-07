#include "gs_death.h"
#include "gs_main_menu.h"
#include "gs_playing.h"

using namespace Urho3D;

gs_death::gs_death() : game_state()
{
    Window* window_=new Window(globals::instance()->context);
    gui_elements.push_back(window_);
    globals::instance()->ui_root->AddChild(window_);

    window_->SetMinSize(500,400);
    window_->SetLayout(LM_VERTICAL,6,IntRect(6,6,6,6));
    window_->SetAlignment(HA_CENTER,VA_CENTER);
    window_->SetName("Window");
    window_->SetColor(Color(.0,.15,.3,.5));

    {
        BorderImage* button=new BorderImage(globals::instance()->context);
        button->SetName("Button");
        button->SetMinHeight(100);
        button->SetStyle("Window");
        button->SetOpacity(0.75);
        {
            Text* t=new Text(globals::instance()->context);
            t->SetFont(globals::instance()->cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),20);
            t->SetHorizontalAlignment(HA_CENTER);
            t->SetVerticalAlignment(VA_CENTER);
            t->SetName("Text");
            t->SetText("You died.");
            button->AddChild(t);
        }
        window_->AddChild(button);
    }
    {
        Button* button = new Button(globals::instance()->context);
        button->SetName("Button");
        button->SetMinHeight(50);
        button->SetStyleAuto();
        button->SetOpacity(0.75);
        {
            Text* t = new Text(globals::instance()->context);
            t->SetFont(globals::instance()->cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),16);
            t->SetHorizontalAlignment(HA_CENTER);
            t->SetVerticalAlignment(VA_CENTER);
            t->SetName("Text");
            t->SetText("Restart level");
            button->AddChild(t);
        }
        window_->AddChild(button);
        SubscribeToEvent(button,E_RELEASED,URHO3D_HANDLER(gs_death,HandleRestartPressed));
    }
    {
        Button* button = new Button(globals::instance()->context);
        button->SetName("Button");
        button->SetMinHeight(50);
        button->SetStyleAuto();
        button->SetOpacity(0.75);
        {
            Text* t = new Text(globals::instance()->context);
            t->SetFont(globals::instance()->cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"),16);
            t->SetHorizontalAlignment(HA_CENTER);
            t->SetVerticalAlignment(VA_CENTER);
            t->SetName("Text");
            t->SetText("Exit to main menu");
            button->AddChild(t);
        }
        window_->AddChild(button);
        SubscribeToEvent(button,E_RELEASED,URHO3D_HANDLER(gs_death,HandleMainMenuPressed));
    }

    GetSubsystem<Input>()->SetMouseVisible(true);
    GetSubsystem<Input>()->SetMouseGrabbed(false);
}

void gs_death::HandleRestartPressed(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData)
{
    GetSubsystem<Input>()->SetMouseVisible(false);
    GetSubsystem<Input>()->SetMouseGrabbed(true);
    globals::instance()->game_states[0].reset(new gs_playing(gs_playing::last_level_filename));
    globals::instance()->game_states.resize(1);
}

void gs_death::HandleMainMenuPressed(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData)
{
    globals::instance()->game_states[0].reset(new gs_main_menu);
    globals::instance()->game_states.resize(1);
}
