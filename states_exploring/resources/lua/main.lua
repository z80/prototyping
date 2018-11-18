
-- set modules paths
package.path = "package.path"..";./modules/?.lua"
BASE_PATH = "./resources/lua"

-- Execute panel parameters
ch, err = loadfile( BASE_PATH.."/panel.lua" )
ch()

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





