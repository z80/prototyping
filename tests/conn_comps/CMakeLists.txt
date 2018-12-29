
set( proj conn_comps )


include_directories( "./src" 
                     "${ROOT}/3dparty/lemon-1.3.1/"
                     "${PROJECT_BINARY_DIR}/3dparty/lemon-1.3.1" )

aux_source_directory( "./src" src )

add_executable( ${proj} ${src} )



