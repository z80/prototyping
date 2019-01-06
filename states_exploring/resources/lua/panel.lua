


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
  -- Celar everything.
  local tt = techTree
  tt:clearNodes()
  tt:clearParts()
  tt:clearCategories()
end

function initTechTree()
  local tt = techTree
  tt:clcearNodes()
  addTestNode()
end

function addTestNode()
  local n = {
    enabled = true, 
    name    = "Test parts node", 
    parents = { "" }, 
    at      = { 0.5, 0.5 }, 
    iconEnabled  = "aaa", 
    iconDisabled = "aaa", 
    iconHovered  = "aaa"
  }
  tt = techTree
  tt:addNode( n )
end












