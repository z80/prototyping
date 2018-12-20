
set( proj states_exploring )



set( LBASE ${OGRE_HOME}/lib )
set( libs ${LBASE}/libOgreMain.so 
          ${LBASE}/libOgreBites.so 
          ${LBASE}/libOgreOverlay.so 
          ${LBASE}/libOgrePaging.so 
          ${LBASE}/libOgreProperty.so 
          ${LBASE}/libOgreRTShaderSystem.so 
          ${LBASE}/libOgreTerrain.so 
          )

# the search paths
include_directories( ${OGRE_INCLUDE_DIRS} 
                     ${OGRE_INCLUDE_DIR} 
                     ${OGRE_INCLUDE_DIR}/Bites 
                     ${OGRE_INCLUDE_DIR}/Overlay 
                     ${OGRE_INCLUDE_DIR}/RTShaderSystem
                     ${ROOT}/3dparty/bullet-2.87/src
                     ${ROOT}/3dparty/btogre/include
                     ${ROOT}/3dparty/oggsound/include
                     ${ROOT}/config_reader/src
                     ${ROOT}/3dparty/lua-5.1.5/src
                     ${ROOT}/3dparty/lua-5.1.5/etc )
link_directories(${OGRE_LIBRARY_DIRS})

# copy essential config files next to our binary where OGRE autodiscovers them
#file(COPY ${OGRE_CONFIG_DIR}/plugins.cfg ${OGRE_CONFIG_DIR}/resources.cfg
#     DESTINATION ${CMAKE_BINARY_DIR})

set( IMGUI ${ROOT}/3dparty/ogreimgui )

set(IMGUI_SRCS ${IMGUI}/imgui/imgui.cpp ${IMGUI}/imgui/imgui_draw.cpp)
set(OGRE_IMGUI_SRCS ${IMGUI}/ImguiManager.cpp ${IMGUI}/ImguiRenderable.cpp)
if(FREETYPE_FOUND)
  include_directories(${IMGUI}/imgui/misc/freetype/)
  list(APPEND IMGUI_SRCS ${IMGUI}/imgui/misc/freetype/imgui_freetype.cpp)
endif()

include_directories( ${IMGUI} ${IMGUI}/imgui/ )

if(FREETYPE_FOUND)
    target_compile_definitions(OgreImgui PRIVATE -DUSE_FREETYPE)
    target_include_directories(OgreImgui PRIVATE ${FREETYPE_INCLUDE_DIRS})
endif()

file( GLOB_RECURSE src "./src/*.cpp" )

include_directories( ./src
                     ./src/bindings 
                     ./src/entity_base 
                     ./src/entity_impl 
                     ./src/panel_constructor 
                     ./src/states_base 
                     ./src/states_impl 
                     ./src/utils )

add_executable( ${proj} ${src} ${IMGUI}/imgui/imgui_demo.cpp )
target_compile_features( ${proj} PRIVATE cxx_range_for )
target_link_libraries( ${proj} OgreImgui BtOgre Plugin_OggSound config_reader lua-5.1.5 )

if(NOT FREETYPE_FOUND)
    set(FREETYPE_FOUND "FALSE")
endif()
message(STATUS "using freetype: ${FREETYPE_FOUND}")

install( TARGETS ${proj} DESTINATION ./ )
install( DIRECTORY "./resources/" DESTINATION ./resources )

add_definitions( -DBT_USE_DOUBLE_PRECISION )




