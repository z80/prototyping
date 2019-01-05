
#include "state_workshop.h"
#include "state_manager.h"
#include "state_intro.h"

#include "entity_factory.h"
#include "camera_ctrl.h"
#include "ImguiManager.h"

#include "config_reader.h"
#include "lua.hpp"
#include "lua_utils.h"

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
    /*groupsInitialized = false;
    panelWidth        = 250;
    itemsPerLine      = 3;*/
}

WorkshopState::~WorkshopState()
{

}

const char * WorkshopState::stateName()
{
    static const char name[] = "workshop";
    return name;
}

void WorkshopState::enter()
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

    paused = false;

    /*if ( !groupsInitialized )
    {
        Config::ConfigReader * cr = StateManager::getSingletonPtr()->getConfigReader();
        if ( cr )
        {
            if ( loadGroups() )
                groupsInitialized = true;
            else
                Ogre::LogManager::getSingletonPtr()->logMessage( "WorkshopState: Failed to load groups" );
        }
    }
    loadLevel();*/
}

bool WorkshopState::frameStarted(const Ogre::FrameEvent& evt)
{
    if ( paused )
        return true;

    ImGui::ShowTestWindow();
    //panelOverlay();
    backToGameOverlay();

    StateManager::getSingletonPtr()->scriptFrameStarted( evt );

    if ( mExitState )
    {
        StateManager::getSingletonPtr()->popState();
        mExitState = false;
    }

    return true;
}

bool WorkshopState::frameEnded(const Ogre::FrameEvent& evt)
{
    StateManager::getSingletonPtr()->getCameraCtrl()->frameRendered( evt );
    return true;
}

bool WorkshopState::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    if ( !disableMouseCtrl )
        StateManager::getSingletonPtr()->getCameraCtrl()->keyPressed( evt );
    if ( evt.keysym.sym == 27 )
        StateManager::getSingletonPtr()->pushState( IntroState::getSingletonPtr() );
    return true;
}

bool WorkshopState::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    if ( !disableMouseCtrl )
        StateManager::getSingletonPtr()->getCameraCtrl()->keyReleased( evt );
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
        StateManager::getSingletonPtr()->getCameraCtrl()->mouseMoved( evt );
    return true;
}

bool WorkshopState::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
    if ( !disableMouseCtrl )
        StateManager::getSingletonPtr()->getCameraCtrl()->mouseWheelRolled( evt );
    return true;
}

bool WorkshopState::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    if ( !disableMouseCtrl )
        StateManager::getSingletonPtr()->getCameraCtrl()->mousePressed( evt );
    return true;
}

bool WorkshopState::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    if ( !disableMouseCtrl )
        StateManager::getSingletonPtr()->getCameraCtrl()->mouseReleased( evt );
    return true;
}





void WorkshopState::createObjects()
{

}

void WorkshopState::destroyObjects()
{
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
            const Ogre::Vector3 at = mCameraNode->getPosition();
            std::ostringstream out;
            out << "cam (" << at.x << ", " << at.y << ", " << at.z << ")";
            ImGui::Text( "%s", out.str().c_str() );
        }
        {
            std::string camMode = StateManager::getSingletonPtr()->getCameraCtrl()->modeStri();
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
    if ( ImGui::Begin( "BackToGame", 0,
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
            StateManager::getSingletonPtr()->popState();

        disableMouseCtrl = ImGui::IsAnyWindowHovered();
    }
    ImGui::End();
}

/*
void WorkshopState::panelOverlay()
{
    const ImVec2 wndSz( 250, ImGui::GetIO().DisplaySize.y );
    ImGui::SetNextWindowBgAlpha( 0.3f ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    const ImVec2 windowPos = ImVec2( 0, 0 );
    const ImVec2 windowPosPivot = ImVec2( 0, 0 );

    ImGui::SetNextWindowPos( windowPos, ImGuiCond_Always, windowPosPivot );
    if ( ImGui::Begin( "Panel", 0,
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoNav )
       )
    {
        const int groupsQty = static_cast<int>( groups.size() );
        const float w = ImGui::GetContentRegionAvailWidth() / static_cast<float>( itemsPerLine );
        for ( int i=0; i<groupsQty; i++ )
        {
            const Group::Group & group = groups[i];
            const int itemsQty = static_cast<int>( group.items.size() );
            const Group::GroupDesc & desc = group.groupDesc;
            if ( ImGui::CollapsingHeader( desc.name.c_str() ) )
            {
                for ( int j=0; j<itemsQty; j++ )
                {
                    const Group::GroupItem & item = group.items[j];
                    const bool notLastInRow = ( ( (j+1) % itemsPerLine ) != 0 ) && ( j != 0 );
                    //ImGui::PushItemWidth( w );
                    panelItem( item, i, j, notLastInRow );
                }
            }
        }

        disableMouseCtrl = ImGui::IsAnyWindowHovered();
    }
    ImGui::End();
}

static void itemClicked( lua_State * L, int group, int item );

void WorkshopState::panelItem( const Group::GroupItem & item, int groupInd, int itemInd,
                               bool notLastInRow )
{
    if ( notLastInRow )
        ImGui::SameLine();
    ImGui::BeginGroup();
    {
        ImGui::Text( "%s", item.name.c_str() );
        const bool clicked = ImGui::ImageButton( (ImTextureID)item.iconHandle, iconSz );
        if ( clicked )
        {
            Config::ConfigReader * cr = StateManager::getSingletonPtr()->getConfigReader();
            lua_State * L = cr->luaState();
            itemClicked( L, groupInd+1, itemInd+1 );
        }
    }
    ImGui::EndGroup();
    if ( ImGui::IsItemHovered() )
    {
        ImGui::BeginTooltip();
        {
            ImGui::PushTextWrapPos( ImGui::GetFontSize() * 135.0f );
            ImGui::TextUnformatted( item.tooltip.c_str() );
            ImGui::PopTextWrapPos();
        }
        ImGui::EndTooltip();
    }
}



static bool getPanelGeometry( lua_State * L, int & width, int & itemsPerLine );
static bool getGroupIconSize( lua_State * L, ImVec2 & sz );
static bool getGroupQty( lua_State * L, int & qty );
static bool getGroup( lua_State * L, int groupInd, Group::GroupDesc & desc );
static bool getItemsQty( lua_State * L, int groupInd, int & qty );
static bool getItem( lua_State * L, int groupInd, int itemInd,
                     std::string & icon, Group::GroupItem & item );
static bool getLevel( lua_State * L, int & level );

bool WorkshopState::loadGroups()
{
    Config::ConfigReader * cr = StateManager::getSingletonPtr()->getConfigReader();
    lua_State * L = cr->luaState();

    getPanelGeometry( L, panelWidth, itemsPerLine );
    getGroupIconSize( L, iconSz );

    int qty;
    if ( !getGroupQty( L, qty ) )
        return false;
    groups.clear();
    groups.resize( qty );
    int itemsQty;
    std::string icon;
    for ( int i=0; i<qty; i++ )
    {
        Group::Group & group = groups[i];
        if ( !getGroup( L, i, group.groupDesc ) )
            return false;
        if ( !getItemsQty( L, i, itemsQty ) )
            return false;
        group.items.resize( itemsQty );
        for ( int j=0; j<itemsQty; j++ )
        {
            if ( !getItem( L, i, j, icon, group.items[j] ) )
                return false;
            group.items[j].icon = icon;
            TexturePtr t = TextureManager::getSingleton().getByName( icon );
            if ( !t )
                t = TextureManager::getSingleton().create( icon, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
            if ( t )
            {
                ResourceHandle hdl = t->getHandle();
                group.items[j].iconHandle = hdl;
            }
            else
                group.items[j].iconHandle = -1;
        }
    }

    getLevel( L, level );
    return true;
}

bool WorkshopState::loadLevel()
{
    Config::ConfigReader * cr = StateManager::getSingletonPtr()->getConfigReader();
    lua_State * L = cr->luaState();
    getLevel( L, level );
}







static bool getPanelGeometry( lua_State * L, int & width, int & itemsPerLine )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "getPanelGeometry" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        Ogre::LogManager::getSingletonPtr()->logWarning(  "getPanelGeometry() not defined" );
        width        = 250;
        itemsPerLine = 3;
        lua_settop( L, top );
        return false;
    }
    const int res = lua_pcall( L, 0, 2, 0 );
    if ( res != 0 )
    {
        reportError( L );
        width        = 250;
        itemsPerLine = 3;
        lua_settop( L, top );
        return false;
    }
    width        = static_cast<int>( lua_tonumber( L, -2 ) );
    itemsPerLine = static_cast<int>( lua_tonumber( L, -1 ) );
    lua_settop( L, top );
}

static bool getGroupIconSize( lua_State * L, ImVec2 & sz )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "getGroupIconSize" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        Ogre::LogManager::getSingletonPtr()->logWarning(  "getGroupIconSize() not defined" );
        sz.x = 32;
        sz.y = 32;
        lua_settop( L, top );
        return false;
    }
    const int res = lua_pcall( L, 0, 2, 0 );
    if ( res != 0 )
    {
        reportError( L );
        sz.x = 32;
        sz.y = 32;
        lua_settop( L, top );
        return false;
    }

    sz.x = lua_tonumber( L, -2 );
    sz.y = lua_tonumber( L, -1 );
    lua_settop( L, top );

    return true;
}

static bool getGroupQty( lua_State * L, int & qty )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "getGroupQty" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        Ogre::LogManager::getSingletonPtr()->logWarning(  "getGroupQty() not defined" );
        qty = 0;
        lua_settop( L, top );
        return false;
    }
    const int res = lua_pcall( L, 0, 1, 0 );
    if ( res != 0 )
    {
        reportError( L );
        qty = 0;
        lua_settop( L, top );
        return false;
    }

    qty = static_cast<int>( lua_tonumber( L, -1 ) );
    lua_settop( L, top );

    return true;
}

static bool getGroup( lua_State * L, int groupInd, Group::GroupDesc & desc )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "getGroup" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        Ogre::LogManager::getSingletonPtr()->logWarning(  "getGroup() not defined" );
        lua_settop( L, top );
        return false;
    }
    lua_pushinteger( L, groupInd+1 );
    const int res = lua_pcall( L, 1, 3, 0 );
    if ( res != 0 )
    {
        reportError( L );
        lua_settop( L, top );
        return false;
    }

    desc.name        = lua_tostring( L, -3 );
    desc.tooltip     = lua_tostring( L, -2 );
    desc.description = lua_tostring( L, -1 );
    lua_settop( L, top );

    return true;
}

static bool getItemsQty( lua_State * L, int groupInd, int & qty )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "getItemsQty" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        Ogre::LogManager::getSingletonPtr()->logWarning(  "getItemsQty() not defined" );
        lua_settop( L, top );
        return false;
    }
    lua_pushinteger( L, groupInd+1 );
    const int res = lua_pcall( L, 1, 1, 0 );
    if ( res != 0 )
    {
        reportError( L );
        lua_settop( L, top );
        return false;
    }

    qty = static_cast<int>( lua_tonumber( L, -1 ) );
    lua_settop( L, top );
    return true;
}

static bool getItem( lua_State * L, int groupInd, int itemInd, std::string & icon, Group::GroupItem & item )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "getItem" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        Ogre::LogManager::getSingletonPtr()->logWarning(  "getItem() not defined" );
        lua_settop( L, top );
        return false;
    }
    lua_pushinteger( L, groupInd+1 );
    lua_pushinteger( L, itemInd+1 );
    const int res = lua_pcall( L, 2, 5, 0 );
    if ( res != 0 )
    {
        reportError( L );
        lua_settop( L, top );
        return false;
    }

    item.name        = lua_tostring( L, -5 );
    item.tooltip     = lua_tostring( L, -4 );
    item.description = lua_tostring( L, -3 );
    icon             = lua_tostring( L, -2 );
    item.level       = static_cast<int>( lua_tonumber( L, -1 ) );
    lua_settop( L, top );

    return true;
}

static bool getLevel( lua_State * L, int & level )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "getLevel" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        Ogre::LogManager::getSingletonPtr()->logWarning(  "getLevel() not defined" );
        lua_settop( L, top );
        return false;
    }
    const int res = lua_pcall( L, 0, 1, 0 );
    if ( res != 0 )
    {
        reportError( L );
        lua_settop( L, top );
        return false;
    }

    level = static_cast<int>( lua_tonumber( L, -1 ) );
    lua_settop( L, top );
    return true;
}

static void itemClicked( lua_State * L, int group, int item )
{
    const int top = lua_gettop( L );
    lua_pushstring( L, "itemClicked" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    if ( lua_isfunction( L, -1 ) == 0 )
    {
        Ogre::LogManager::getSingletonPtr()->logWarning(  "itemClicked() not defined" );
        lua_settop( L, top );
        return;
    }
    lua_pushinteger( L, group );
    lua_pushinteger( L, item );
    const int res = lua_pcall( L, 2, 0, 0 );
    if ( res != 0 )
    {
        reportError( L );
        lua_settop( L, top );
        return;
    }

    lua_settop( L, top );
    return;
}
*/




