
#include "state_workshop.h"
#include "state_manager.h"
#include "state_intro.h"

#include "entity_factory.h"
#include "camera_ctrl.h"
#include "ImguiManager.h"

#include "config_reader.h"
#include "lua.hpp"

using namespace Ogre;

static WorkshopState g_is;

template<> WorkshopState* Singleton<WorkshopState>::msSingleton = 0;

WorkshopState::WorkshopState()
    : State(),
      Ogre::Singleton<WorkshopState>()
{
    mRoot     = 0;
    mSceneMgr = 0;
    mViewport = 0;
    mCamera   = 0;
    mCameraNode = 0;
    mExitState = false;
    paused     = false;
    disableMouseCtrl = false;
}

WorkshopState::~WorkshopState()
{

}

void WorkshopState::enter()
{
    if ( !paused )
    {
        mRoot = Root::getSingletonPtr();
        mSceneMgr = StateManager::getSingletonPtr()->getSceneManager();
        mCamera = mSceneMgr->getCamera( "IntroCamera" );
        mViewport = StateManager::getSingletonPtr()->getRenderWindow()->getViewport( 0 );

        mCamera->setFixedYawAxis( false );

        try
        {
            mCameraNode = mSceneMgr->getSceneNode( "CameraNode" );
        }
        catch ( ... )
        {
            mCameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "CameraNode" );
            mCameraNode->attachObject( mCamera );
        }

        createObjects();
    }

    mViewport->setBackgroundColour(ColourValue(0.7, 0.7, 0.4));
    StateManager::getSingletonPtr()->setMouseVisible( true );
    mExitState = false;


    resume();
}

void WorkshopState::exit()
{
    pause();

    destroyObjects();
}

void WorkshopState::pause()
{
    //mSceneMgr->setSkyBox( false, "Examples/CloudyNoonSkyBox" );

    paused = true;
}

void WorkshopState::resume()
{
    //mSceneMgr->setSkyBox( true, "Examples/CloudyNoonSkyBox" );
    mSceneMgr->setAmbientLight( Ogre::ColourValue( 0.5, 0.5, 0.5 ) );

    CameraCtrl::getSingletonPtr()->setCameraNode( mCameraNode );
    CameraCtrl::getSingletonPtr()->setTargetNode( cube->sceneNode );

    paused = false;
}

bool WorkshopState::frameStarted(const Ogre::FrameEvent& evt)
{
    if ( paused )
        return true;

    ImGui::ShowTestWindow();
    backToGameOverlay();
    debugOverlay();

    if ( mExitState )
    {
        StateManager::getSingletonPtr()->popState();
        mExitState = false;
    }

    return true;
}

bool WorkshopState::frameEnded(const Ogre::FrameEvent& evt)
{
    CameraCtrl::getSingletonPtr()->frameRendered( evt );
    return true;
}

bool WorkshopState::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->keyPressed( evt );
    if ( evt.keysym.sym == 27 )
    {
        //StateManager::getSingletonPtr()->popState();
        StateManager::getSingletonPtr()->pushState( IntroState::getSingletonPtr() );
    }
}

bool WorkshopState::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->keyReleased( evt );
    return true;
}

bool WorkshopState::touchMoved(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool WorkshopState::touchPressed(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool WorkshopState::touchReleased(const OgreBites::TouchFingerEvent& evt)
{
    return true;
}

bool WorkshopState::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->mouseMoved( evt );
    return true;
}

bool WorkshopState::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->mouseWheelRolled( evt );
    return true;
}

bool WorkshopState::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->mousePressed( evt );
    return true;
}

bool WorkshopState::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    if ( !disableMouseCtrl )
        CameraCtrl::getSingletonPtr()->mouseReleased( evt );
    return true;
}





void WorkshopState::createObjects()
{

}

void WorkshopState::destroyObjects()
{
    CameraCtrl::getSingletonPtr()->setCameraNode( 0 );
    CameraCtrl::getSingletonPtr()->setTargetNode( 0 );
}

void WorkshopState::debugOverlay()
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
    }
    ImGui::End();

}

void WorkshopState::backToGameOverlay()
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
        if ( ImGui::Button( "Leave workshop", ImVec2( 130, 30 ) ) )
        {
            mExitState = true;
        }

        disableMouseCtrl = ImGui::IsAnyWindowHovered();
    }
    ImGui::End();
}



static void getGroupIconSize( lua_State * L, ImVec2 & sz );
static void getGroupQty( lua_State * L, int & qty );
static void getGroup( lua_State * L, int groupInd, Group::GroupDesc & desc );
static void getItemsQty( lua_State * L, int groupInd, int & qty );
static void getItem( lua_State * L, int groupInd, int itemInd, Group::GroupItem & item );
static void getLevel( lua_State * L, int & level );

bool WorkshopState::loadGroups()
{
    Config::ConfigReader * cr = StateManager::getSingletonPtr()->getConfigReader();
    const bool res = cr->openFile( "./main.lua" );
    if ( !res )
        return false;
    lua_State * L = cr->luaState();

    getGroupIconSize( L, iconSz );

    int qty;
    getGroupQty( L, qty );
    groups.clear();
    groups.resize( qty );
    int itemsQty;
    for ( int i=0; i<qty; i++ )
    {
        Group::Group & group = groups[i];
        getGroup( L, i, group.groupDesc );
        getItemsQty( L, i, itemsQty );
        group.items.resize( itemsQty );
        for ( int j=0; j<itemsQty; j++ )
            getItem( L, i, j, group.items[j] );
    }

    getLevel( L, level );
}






static void getGroupIconSize( lua_State * L, ImVec2 & sz )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "getGroupIconSize" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        sz.x = 32;
        sz.y = 32;
        lua_settop( L, top );
        return;
    }
    const int res = lua_pcall( L, 0, 2, -1 );
    if ( res != 0 )
    {
        sz.x = 32;
        sz.y = 32;
        lua_settop( L, top );
        return;
    }

    sz.x = lua_tonumber( L, -2 );
    sz.y = lua_tonumber( L, -1 );
    lua_settop( L, top );
}

static void getGroupQty( lua_State * L, int & qty )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "getGroupQty" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        qty = 0;
        lua_settop( L, top );
        return;
    }
    const int res = lua_pcall( L, 0, 1, -1 );
    if ( res != 0 )
    {
        qty = 0;
        lua_settop( L, top );
        return;
    }

    qty = static_cast<int>( lua_tonumber( L, -1 ) );
    lua_settop( L, top );
}

static void getGroup( lua_State * L, int groupInd, Group::GroupDesc & desc )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "getGroup" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        desc.name        = "error";
        desc.tooltip     = "error";
        desc.description = "error";
        lua_settop( L, top );
        return;
    }
    lua_pushinteger( L, groupInd+1 );
    const int res = lua_pcall( L, 1, 3, -1 );
    if ( res != 0 )
    {
        desc.name        = "error";
        desc.tooltip     = "error";
        desc.description = "error";
        lua_settop( L, top );
        return;
    }

    desc.name        = lua_tostring( L, -3 );
    desc.tooltip     = lua_tostring( L, -2 );
    desc.description = lua_tostring( L, -1 );
    lua_settop( L, top );
}

static void getItemsQty( lua_State * L, int groupInd, int & qty )
{

}

static void getItem( lua_State * L, int groupInd, int itemInd, Group::GroupItem & item )
{

}

static void getLevel( lua_State * L, int & level )
{

}





