cmake_minimum_required (VERSION 2.8.12)
project(OgreImgui)

#set( CMAKE_MODULE_PATH "${OgreImgui_SOURCE_DIR}/cmake" )
include( "${CMAKE_SOURCE_DIR}/settings.cmake" )
set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )
# specify which version you need
find_package(OGRE 1.11 REQUIRED)
find_package(Freetype QUIET)

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
                     ${OGRE_INCLUDE_DIR}/Bites 
                     ${OGRE_INCLUDE_DIR}/RTShaderSystem )
link_directories(${OGRE_LIBRARY_DIRS})

# copy essential config files next to our binary where OGRE autodiscovers them
#file(COPY ${OGRE_CONFIG_DIR}/plugins.cfg ${OGRE_CONFIG_DIR}/resources.cfg
#     DESTINATION ${CMAKE_BINARY_DIR})

set(IMGUI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp)
set(OGRE_IMGUI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/ImguiManager.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ImguiRenderable.cpp)
if(FREETYPE_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/freetype/)
  list(APPEND IMGUI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/freetype/imgui_freetype.cpp)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui/)
add_library(OgreImgui STATIC ${IMGUI_SRCS} ${OGRE_IMGUI_SRCS})
target_compile_features( OgreImgui PRIVATE cxx_range_for)
target_link_libraries(OgreImgui PUBLIC ${OGRE_LIBRARIES} ${libs} )

if(FREETYPE_FOUND)
    target_compile_definitions(OgreImgui PRIVATE -DUSE_FREETYPE)
    target_include_directories(OgreImgui PRIVATE ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(OgreImgui PRIVATE ${FREETYPE_LIBRARIES})
endif()

add_executable(Example Example.cpp ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp)
target_compile_features( Example PRIVATE cxx_range_for)
target_link_libraries(Example OgreImgui)

if(NOT FREETYPE_FOUND)
    set(FREETYPE_FOUND "FALSE")
endif()
message(STATUS "using freetype: ${FREETYPE_FOUND}")





