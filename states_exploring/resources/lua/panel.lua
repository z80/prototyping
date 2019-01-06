


--[[ 
Tech panel geometry parameters 
]]

techPanel = 
{
    itemsPerLine = 3, 
    iconSz       = 128, 
    panelSz      = 256, 
    alpha        = 0.3
}


techTree = core.techTreeCreate()

function initTechTree()
  local tt = techTree

  tt:clearNodes()
  addTestNode( tt )
  addAnotherNode( tt )

  tt:clearCategories()
  addCategoryTest( tt )
  addCategoryAnother( tt )

  tt:clearParts()
  addPartCube( tt )
end





function addTestNode( tt )
  local n = {
    enabled = true, 
    name    = "Test parts node", 
    parents = { "" }, 
    at      = { 0.4, 0.5 }, 
    iconEnabled  = "aaa", 
    iconDisabled = "aaa", 
    iconHovered  = "aaa"
  }
  tt:addNode( n )
end

function addAnotherNode( tt )
  local n = {
    enabled = false, 
    name    = "Another node", 
    parents = { "Test parts node" }, 
    at      = { 0.6, 0.5 }, 
    iconEnabled  = "aaa", 
    iconDisabled = "aaa", 
    iconHovered  = "aaa"
  }
  tt:addNode( n )
end






function addCategoryTest( tt )
  local c = {
    name        = "Test category", 
    description = "This is a test category description. It contatins test blocks.", 
    tooltip     = "Test category toolitp", 
    icon        = "aaa"
  }
  tt:addCategory( c )
end

function addCategoryAnother( tt )
  local c = {
    name        = "Another category", 
    description = "This is another category description. It contatins nothing :)", 
    tooltip     = "Another category tooltip", 
    icon        = "aaa"
  }
  tt:addCategory( c )
end






function addPartCube( tt )
  local p = {
    name        = "cube", 
    description = "Dumb cube", 
    tooltip     = "This is just a dumb cube", 
    neededNode  = "Test parts node", 
    category    = "Test category", 
    icon        = "aaa", 
    connections = {
      { r = { 0.0, 0.5, 0.0 }, 
        q = { 1.0, 0.0, 0.0, 0.0 }, 
        anySurface = false, 
        spin       = false, 
        tilt       = false
      }, 
      { r = { 0.0,-0.5, 0.0 }, 
        q = { 1.0, 0.0, 0.0, 0.0 }, 
        anySurface = false, 
        spin       = false, 
        tilt       = false 
      }
    }
  }

  tt:addPart( p )
end












