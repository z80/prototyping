
function getGroupIconSize()
    return 64, 64
end

function getGroupQty()
    return 2
end

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

function getGroup( index )
    local g = groups[index]
    return g.name, g.tooltip, g.description
end

function getItemsQty( groupInd )
    local qty = 1
    return qty
end

function getItem( groupInd, itemInd )
    return "item item", "item tooltip", "item description", "SphereTexture.png", 1
end







