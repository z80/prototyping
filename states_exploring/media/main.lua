
groups = {
    {
        name    = "group 1", 
        tooltip = "Tooltip 1", 
        description = "Description 1"
    }, 
    {
        name = "group 2", 
        tooltip = "Tooltip 2", 
        description = "Description 2"
    }
}

function getPanelGeometry()
    return 220, 3
end

function getGroupIconSize()
    return 64, 64
end

function getGroupQty()
    return #groups
end

function getGroup( index )
    local g = groups[index]
    return g.name, g.tooltip, g.description
end

function getItemsQty( groupInd )
    local qty = (groupInd == 1) and 12 or 5
    return qty
end

function getItem( groupInd, itemInd )
    if ( groupInd < 2 ) then
        return "This is " .. tostring(itemInd), "item tooltip", "item description", "SphereTexture.png", 1
    end
    return "This is " .. tostring(itemInd), "item tooltip", "item description", "Another.png", 1
end

function stateEntered( name )
    core.log( "Entered " .. name )
end
function stateLeft( name )
    core.log( "Left " .. name )
end
function statePaused( name )
    core.log( "Paused " .. name )
end
function stateResumed( name )
    core.log( "Resumed " .. name )
end


function frameStarted( dt )
    core.log( "dt = " .. tostring( dt ) )
end





