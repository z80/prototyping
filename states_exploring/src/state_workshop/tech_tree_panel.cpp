
#include "tech_tree_panel.h"
#include "state_manager.h"
#include "config_reader.h"
#include "tech_tree.h"

namespace Entity
{

TechTreePanel::TechTreePanel()
{
    panelSz      = 250;
    itemsPerLine = 3;
    iconSz       = 128;
    alpha        = 0.3;
    hovered      = false;

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
}


void TechTreePanel::drawBackToGamePanel()
{
    const ImVec2 wndSz( 150, 50 );
    ImGui::SetNextWindowBgAlpha( alpha ); // Transparent background
    ImGui::SetNextWindowSizeConstraints( wndSz, wndSz );
    const ImVec2 windowPos = ImVec2( -10.0,
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
            cb->part( item.name );
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

