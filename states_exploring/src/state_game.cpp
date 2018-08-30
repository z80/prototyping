
#include "state_game.h"
#include "state_manager.h"
#include "state_intro.h"
#include "state_workshop.h"

#include "entity_factory.h"
#include "camera_ctrl.h"
#include "ImguiManager.h"

using namespace Ogre;

static GameState g_is;

template<> GameState* Singleton<GameState>::msSingleton = 0;

GameState::GameState()
    : State(),
      Ogre::Singleton<GameState>()
{
    mRoot     = 0;
    mSceneMgr = 0;
    mViewport = 0;
    mCamera   = 0;
    mCameraNode = 0;
    doDebugDraw = false;
    mExitState = false;
    paused     = false;
    disableMouseCtrl = false;
    toWorkshopMode = false;

    world  = 0;
    //plane = 0;
    cube   = 0;
    planet = 0;
}

GameState::~GameState()
{

}

const char * GameState::stateName()
{
    static const char name[] = "game";
    return name;
}

void GameState::enter()
{
    if ( !paused )
    {
        mRoot = Root::getSingletonPtr();
        mSceneMgr = StateManager::getSingletonPtr()->getSceneManager();
        mCamera = StateManager::getSingletonPtr()->getCamera();
        mCamera->setNearClipDistance( 0.0001 );
        mViewport = StateManager::getSingletonPtr()->getRenderWindow()->getViewport( 0 );
        mCameraNode = mCamera->getParentSceneNode();

        createObjects();
    }

    mViewport->setBackgroundColour(ColourValue(0.7, 0.7, 0.4));
    StateManager::getSingletonPtr()->setMouseVisible( true );
    mExitState = false;


    resume();
}

void GameState::exit()
{
    pause();

    destroyObjects();
}

void GameState::pause()
{
    //mSceneMgr->setSkyBox( false, "Examples/CloudyNoonSkyBox" );

    paused = true;
}

void GameState::resume()
{
    //mSceneMgr->setSkyBox( true, "Examples/CloudyNoonSkyBox" );
    mSceneMgr->setAmbientLight( Ogre::ColourValue( 0.5, 0.5, 0.5 ) );

    CameraCtrl::getSingletonPtr()->setCameraNode( mCameraNode );
    CameraCtrl::getSingletonPtr()->setTargetNode( cube->sceneNode );

    paused = false;
}

bool GameState::frameStarted(const Ogre::FrameEvent& evt)
{
    if ( paused )
        return true;

    modesOverlay();
    debugOverlay();
    ImGui::ShowTestWindow();
    if ( world )
    {
        planet->addForces( *cube );
        world->frameStarted( evt, doDebugDraw );
    }
    if ( toWorkshopMode )
    {
        toWorkshopMode = false;
        StateManager::getSingletonPtr()->pushState( WorkshopState::getSingletonPtr() );

    }

    return true;
}

bool GameState::frameEnded(const Ogre::FrameEvent& evt)
{
    CameraCtrl::getSingletonPtr()->frameRendered( evt );
    return true;
}

bool GameState::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->keyPressed( evt );
    if ( evt.keysym.sym == 27 )
    {
        //StateManager::getSingletonPtr()->popState();
        StateManager::getSingletonPtr()->pushState( IntroState::getSingletonPtr() );
    }
}

bool GameState::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->keyReleased( evt );
    return true;
}

bool GameState::touchMoved(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool GameState::touchPressed(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool GameState::touchReleased(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool GameState::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->mouseMoved( evt );
    return true;
}

bool GameState::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->mouseWheelRolled( evt );
    return true;
}

bool GameState::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->mousePressed( evt );
    return true;
}

bool GameState::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->mouseReleased( evt );
    return true;
}





void GameState::createObjects()
{
    world = dynamic_cast<Entity::EntityWorld *>(
                Entity::EntityFactory::getSingletonPtr()->create( "world" ) );
    //plane = dynamic_cast<Entity::EntityPart *>(
    //            Entity::EntityFactory::getSingletonPtr()->create( "plane" ) );
    cube  = dynamic_cast<Entity::EntityPart *>(
                Entity::EntityFactory::getSingletonPtr()->create( "cube" ) );
    planet = dynamic_cast<Entity::EntityPlanet *>(
                Entity::EntityFactory::getSingletonPtr()->create( "planet" ) );

    // Debugging object to see at least something.
    {
        Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
        Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        ogreNode->attachObject(ogreEntity);
        ogreNode->setPosition( 0.0, 0.0, -10.0 );
        ogreNode->setScale( 0.1, 0.1, 0.1 );
        ogreNode->setVisible( true );
    }
    {
        Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
        Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        ogreNode->attachObject(ogreEntity);
        ogreNode->setPosition( 0.0, 0.0, 10.0 );
        ogreNode->setScale( 0.1, 0.1, 0.1 );
        ogreNode->setVisible( true );
    }
    {
        Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
        Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        ogreNode->attachObject(ogreEntity);
        ogreNode->setPosition( 0.0, 10.0, 0.0 );
        ogreNode->setScale( 0.1, 0.1, 0.1 );
        ogreNode->setVisible( true );
    }
    {
        Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
        Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        ogreNode->attachObject(ogreEntity);
        ogreNode->setPosition( 0.0, -10.0, 0.0 );
        ogreNode->setScale( 0.1, 0.1, 0.1 );
        ogreNode->setVisible( true );
    }
    {
        Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
        Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        ogreNode->attachObject(ogreEntity);
        ogreNode->setPosition( 10.0, 0.0, 0.0 );
        ogreNode->setScale( 0.1, 0.1, 0.1 );
        ogreNode->setVisible( true );
    }
    {
        Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
        Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        ogreNode->attachObject(ogreEntity);
        ogreNode->setPosition( -10.0, 0.0, 0.0 );
        ogreNode->setScale( 0.1, 0.1, 0.1 );
        ogreNode->setVisible( true );
    }
}

void GameState::destroyObjects()
{
    CameraCtrl::getSingletonPtr()->setCameraNode( 0 );
    CameraCtrl::getSingletonPtr()->setTargetNode( 0 );

    if ( mCameraNode )
        mSceneMgr->destroySceneNode( mCameraNode );
    if ( planet )
        delete planet;
    if ( cube )
        delete cube;
    //if ( plane )
    //    delete plane;
    if ( world )
        Entity::EntityWorld::deleteWorld();
}

void GameState::debugOverlay()
{
    const ImVec2 wndSz( 340, 120 );
    ImGui::SetNextWindowBgAlpha( 0.3f ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    const ImVec2 windowPos = ImVec2( 10, 10 );
    const ImVec2 windowPosPivot = ImVec2( 0, 0 );

    ImGui::SetNextWindowPos( windowPos, ImGuiCond_Always, windowPosPivot );
    if ( ImGui::Begin( "Choose", 0,
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoNav )
       )
    {
        {
            const Ogre::Vector3 at = cube->sceneNode->getPosition();
            std::ostringstream out;
            out << "cube (" << at.x << ", " << at.y << ", " << at.z << ")";
            ImGui::Text( "%s", out.str().c_str() );
        }
        /*{
            const Ogre::Vector3 at = plane->sceneNode->getPosition();
            std::ostringstream out;
            out << "plane (" << at.x << ", " << at.y << ", " << at.z << ")";
            ImGui::Text( "%s", out.str().c_str() );
        }*/
        {
            const Ogre::Vector3 at = mCameraNode->getPosition();
            std::ostringstream out;
            out << "cam (" << at.x << ", " << at.y << ", " << at.z << ")";
            ImGui::Text( "%s", out.str().c_str() );
        }
        {
            std::string camMode = CameraCtrl::getSingletonPtr()->modeStri();
            ImGui::Text( "camera mode: \"%s\"", camMode.c_str() );
        }

        disableMouseCtrl = ImGui::IsAnyWindowHovered();

        ImGui::End();
    }


    if (ImGui::BeginMainMenuBar())
    {
        if ( ImGui::BeginMenu("Debug") )
        {
            ImGui::MenuItem( "Debug draw", 0, &doDebugDraw );
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

}

void GameState::modesOverlay()
{
    const ImVec2 wndSz( 150, 50 );
    ImGui::SetNextWindowBgAlpha( 0.3f ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    const ImVec2 windowPos = ImVec2( ImGui::GetIO().DisplaySize.x - wndSz.x - 10,
                                      10 );
    const ImVec2 windowPosPivot = ImVec2( 0, 0 );

    ImGui::SetNextWindowPos( windowPos, ImGuiCond_Always, windowPosPivot );
    if ( ImGui::Begin( "Modes", 0,
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoNav )
       )
    {
        if ( ImGui::Button( "Workshop", ImVec2( 130, 30 ) ) )
            StateManager::getSingletonPtr()->pushState( WorkshopState::getSingletonPtr() );

        disableMouseCtrl = ImGui::IsAnyWindowHovered();
    }
    ImGui::End();
}




