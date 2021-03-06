
#include "tech_tree_panel.h"
#include "state_manager.h"
#include "config_reader.h"
#include "tech_tree.h"
#include "design_manager.h"
#include "design_construction.h"

namespace Osp
{

TechTreePanel::TechTreePanel()
{
    panelSz      = 250;
    itemsPerLine = 3;
    iconSz       = 128;
    alpha        = 0.3;
    hovered      = false;
    tooltip      = "Place hints about current state here";
    hoveredDesignIndex  = -1;
    savingDesign = false;

    initPanelGeometry();
}

TechTreePanel::~TechTreePanel()
{

}

void TechTreePanel::initPanelGeometry()
{
    Config::ConfigReader * cr = StateManager::getSingletonPtr()->getConfigReader();
    cr->readInt( "techPanel.itemsPerLine", itemsPerLine, itemsPerLine );
    cr->readInt( "techPanel.iconSz",       iconSz,       iconSz );
    cr->readInt( "techPanel.panelSz",      panelSz,      panelSz );
    Config::ConfigReader::Float a = alpha;
    cr->readFloat( "techPanel.alpha",      a,            a );
    alpha = static_cast<Ogre::Real>( a );
}

void TechTreePanel::updateAllowedParts()
{
    TechTree * tt = StateManager::getSingletonPtr()->getTechTree();
    tt->generatePanelContent();

    // Re-read/load part icon textures here.
    std::vector<PartDesc> & pds = tt->getPartDescs();
    const size_t qty = pds.size();
    for ( size_t i=0; i<qty; i++ )
    {
        PartDesc & pd = pds[i];
        try
        {
            Ogre::TexturePtr t_ptr = Ogre::TextureManager::getSingletonPtr()->getByName( pd.icon );
            if ( !t_ptr )
                t_ptr = Ogre::TextureManager::getSingletonPtr()->load( pd.icon, "General" );
            if ( t_ptr )
                pd.iconHandle = t_ptr->getHandle();
            else
                pd.iconHandle = -1;
        } catch ( ... )
        {
            Ogre::TexturePtr t_ptr = Ogre::TextureManager::getSingletonPtr()->load( pd.icon, "General" );
            pd.iconHandle = t_ptr->getHandle();
        }
    }
}

void TechTreePanel::setTooltip( const Ogre::String & stri )
{
    tooltip = stri;
}


void TechTreePanel::drawBackToGamePanel()
{
    const ImVec2 wndSz( 150, 50 );
    ImGui::SetNextWindowBgAlpha( alpha ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    const ImVec2 windowPos = ImVec2( ImGui::GetIO().DisplaySize.x-10.0,
                                      10.0 );
    const ImVec2 windowPosPivot = ImVec2( 1.0, 0.0 );

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
    }
    ImGui::End();
}

static void panelItem( const PartDesc & item,
                       int iconSz,
                       bool notLastInRow,
                       TechTreePanelCallback * cb );

void TechTreePanel::drawTechPanel( TechTreePanelCallback * cb )
{
    TechTree * tt = StateManager::getSingletonPtr()->getTechTree();
    if ( !tt )
        return;

    const std::vector<CategoryDesc> & panelContent = tt->getPanelContent();
    const std::vector<PartDesc> & partDescs = tt->getPartDescs();

    const ImVec2 wndSz( panelSz, ImGui::GetIO().DisplaySize.y );
    ImGui::SetNextWindowBgAlpha( alpha ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    const ImVec2 windowPos = ImVec2( 0, 20 );
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
        const size_t groupsQty = panelContent.size();
        const float w = ImGui::GetContentRegionAvailWidth() / static_cast<float>( itemsPerLine );
        for ( size_t i=0; i<groupsQty; i++ )
        {
            const CategoryDesc & cat = panelContent[i];
            const size_t itemsQty = cat.items.size();
            if ( ImGui::CollapsingHeader( cat.name.c_str() ) )
            {
                for ( int j=0; j<itemsQty; j++ )
                {
                    const int partInd = cat.items[j];
                    const PartDesc & p = partDescs[partInd];
                    const bool notLastInRow = ( ( (j+1) % itemsPerLine ) != 0 ) && ( j != 0 );
                    //ImGui::PushItemWidth( w );
                    panelItem( p, iconSz, notLastInRow, cb );
                }
            }
        }

        hovered = ImGui::IsWindowHovered( ImGuiHoveredFlags_AnyWindow );
    }
    ImGui::End();
}

void TechTreePanel::drawTipPanel()
{
    const ImVec2 wndSz( 350, 50 );
    ImGui::SetNextWindowBgAlpha( alpha ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    const ImVec2 windowPos = ImVec2( ImGui::GetIO().DisplaySize.x-wndSz.x,
                                     ImGui::GetIO().DisplaySize.y-wndSz.y );
    const ImVec2 windowPosPivot = ImVec2( 0.0, 0.0 );

    ImGui::SetNextWindowPos( windowPos, ImGuiCond_Always, windowPosPivot );
    if ( ImGui::Begin( "Tooltip", 0,
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoNav )
       )
    {
        ImGui::TextWrapped( "%s", tooltip.c_str() );
    }
    ImGui::End();
}

void TechTreePanel::drawMainMenu( DesignManager * dm )
{
    if ( ImGui::BeginMainMenuBar() )
    {
        if ( dm )
        {
            if ( ImGui::BeginMenu("Design") )
            {
                if ( ImGui::MenuItem( "Save design", 0, false, true ) )
                    savingDesign = true;
                ImGui::EndMenu();
            }

            hoveredDesignIndex = -1;

            std::vector<DesignManager::DesignItem> & designs = dm->designs;
            const size_t qty = designs.size();
            if ( ImGui::BeginMenu("All") )
            {
                for ( size_t i=0; i<qty; i++ )
                {
                    if ( ImGui::MenuItem( designs[i].name.c_str(), 0, false, true ) )
                    {
                        designItem = designs[i];
                    }
                    const bool hovered = ImGui::IsItemHovered( ImGuiHoveredFlags_None );
                    if ( hovered )
                        hoveredDesignIndex = (int)i;
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }
}

void TechTreePanel::drawDesignView( DesignManager * dm )
{
    if ( hoveredDesignIndex < 0 )
        return;

    const ImVec2 wndSz( 250, 600 );
    ImGui::SetNextWindowBgAlpha( alpha ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    const ImVec2 windowPos = ImVec2( (ImGui::GetIO().DisplaySize.x-wndSz.x)/2,
                                     (ImGui::GetIO().DisplaySize.y-wndSz.y)/2 );
    const ImVec2 windowPosPivot = ImVec2( 0.0, 0.0 );

    ImGui::SetNextWindowPos( windowPos, ImGuiCond_Always, windowPosPivot );
    if ( ImGui::Begin( "Design view", 0,
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoNav )
       )
    {
        const DesignManager::DesignItem & di = dm->designItem( hoveredDesignIndex );
        ImGui::TextWrapped( "%s", di.name.c_str() );
        ImGui::TextWrapped( "%s", di.desc.c_str() );
    }
    ImGui::End();

}

void TechTreePanel::drawDesignSave( DesignManager * dm, DesignConstruction * dc )
{
    if ( !savingDesign )
        return;

    const ImVec2 wndSz( 250, 600 );
    ImGui::SetNextWindowBgAlpha( alpha ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    const ImVec2 windowPos = ImVec2( (ImGui::GetIO().DisplaySize.x-wndSz.x)/2,
                                     (ImGui::GetIO().DisplaySize.y-wndSz.y)/2 );
    const ImVec2 windowPosPivot = ImVec2( 0.0, 0.0 );

    ImGui::SetNextWindowPos( windowPos, ImGuiCond_Always, windowPosPivot );
    if ( ImGui::Begin( "Design view", 0,
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoNav )
       )
    {
        char name[128];
        char desc[4096];
        sprintf( name, "%s", this->name.c_str() );
        sprintf( desc, "%s", this->desc.c_str() );
        ImGui::InputText( "Design name: ", name, sizeof(name) );
        ImGui::InputTextMultiline( "Description: ", desc, sizeof(desc),
                                   ImVec2(-1, ImGui::GetTextLineHeight() * 16) );
        this->name = name;
        this->desc = desc;
        if ( ImGui::Button( "Save" ) )
        {
            Design d = dc->design();
            dm->saveDesign( name, desc, d );
            savingDesign = false;
        }
        ImGui::SameLine();
        if ( ImGui::Button( "Cancel" ) )
        {
            savingDesign = false;
        }
    }
    ImGui::End();
}

const bool TechTreePanel::isHovered() const
{
    return hovered;
}

static void panelItem( const PartDesc & item,
                       int iconSz,
                       bool notLastInRow,
                       TechTreePanelCallback * cb )
{
    if ( notLastInRow )
        ImGui::SameLine();
    ImGui::BeginGroup();
    {
        const ImVec2 sz( iconSz, iconSz );
        const bool clicked = ImGui::ImageButton( (ImTextureID)item.iconHandle, sz );
        if ( clicked )
            cb->block( item.name );
        ImGui::Text( "%s", item.name.c_str() );
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





}

