
set( target "config_reader" )

include_directories( "./src"
                     ${ROOT}/libvo2/src
                     ${ROOT}/3dparty/lua-5.1.5/src 
                     ${ROOT}/3dparty/lua-5.1.5/etc )

aux_source_directory( "./src" src )
add_library( ${target} STATIC ${src} )
target_link_libraries( ${target} lua-5.1.5 )





