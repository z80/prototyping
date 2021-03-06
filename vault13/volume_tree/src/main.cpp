
#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"
#include "OgreTrays.h"
#include "OgreAdvancedRenderControls.h"

#include "volume.h"
#include "lod_tree.h"
#include "sphere_source.h"

#include <iostream>

using namespace Ogre;
using namespace OgreBites;

class BasicTutorial1
        : public ApplicationContext
        , public InputListener
        , public TrayListener
{
public:
    BasicTutorial1();
    virtual ~BasicTutorial1() {}

    void setup();
    void shutdown();

    bool frameRenderingQueued( const FrameEvent & evt );
    //bool frameStarted( const Ogre::FrameEvent & evt );
    bool frameEnded( const Ogre::FrameEvent & evt );

    bool keyPressed( const KeyboardEvent & evt );
    bool keyReleased( const KeyboardEvent & evt );
    bool mouseMoved( const MouseMotionEvent & evt );
    bool mousePressed( const MouseButtonEvent & evt );
    bool mouseReleased( const MouseButtonEvent & evt );

private:
    CameraMan * cMan;
    TrayManager * mTrayManager;
    AdvancedRenderControls * mAdvancedControls;
    //Volume2 * volume;
    LodTree::Tree * tree;
};


BasicTutorial1::BasicTutorial1()
    : ApplicationContext("OgreTutorialApp")
{
    cMan = 0;
    mTrayManager = 0;
    mAdvancedControls = 0;
    tree = 0;
}

void BasicTutorial1::shutdown()
{
    if ( cMan )
        delete cMan;
    if ( mAdvancedControls )
        delete mAdvancedControls;
    if ( mTrayManager )
        delete mTrayManager;
    delete tree;
}


void BasicTutorial1::setup()
{
    // do not forget to call the base first
    ApplicationContext::setup();
    addInputListener(this);
    // To lock mouse to the window.
    this->setWindowGrab();

    // get a pointer to the already created root
    Root* root = getRoot();
    SceneManager* scnMgr = root->createSceneManager();

    // register our scene with the RTSS
    RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    // -- tutorial section start --
    //! [turnlights]
    scnMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
    //! [turnlights]

    //! [newlight]
    Light* light = scnMgr->createLight("MainLight");
    SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    lightNode->attachObject(light);
    //! [newlight]

    //! [lightpos]
    lightNode->setPosition(20, 80, 50);
    //! [lightpos]

    //! [camera]
    SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();

    // create the camera
    Camera* cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);
    camNode->setPosition(0, 0, 140);

    // Convenience camera control
    cMan = new CameraMan( camNode );
    cMan->setStyle( CS_MANUAL );
    //cMan->

    // and tell it to render into the main window
    getRenderWindow()->addViewport(cam);
    //! [camera]

    //! [cameramove]
    camNode->setPosition( 0.0, 0.0, 5005.0 );
    //! [cameramove]

    // -- tutorial section end --

    Ogre::RenderWindow * wnd = getRenderWindow();
    mTrayManager = new TrayManager( "my_tray", wnd, this );
    mTrayManager->showCursor();

    mAdvancedControls = new AdvancedRenderControls( mTrayManager, cam );

    this->locateResources();

    mTrayManager->showFrameStats( TL_BOTTOMLEFT );
    //mTrayManager->showAll();
    //mTrayManager->showTrays();

    this->loadResources();

    OverlaySystem * os = this->getOverlaySystem();
    scnMgr->addRenderQueueListener( os );


    // ****************************************
    LodTree::TreeParams params;
    SphereSource        *ss = new SphereSource( 4095.0, Vector3() );

    {
        params.at        = Vector3( 0.0, 0.0, 4095.0 );
        params.halfSz    = 4200.0;
        params.src       = ss;
        params.maxLevel  = 8;
        params.baseError = [&](int level)
        {
            if ( level<8 )
                return 700.0;
            else
                return 10.0;
            /*if ( level<2 )
                return 300.0;
            else if ( level<4 )
                return 100.0;
            else
                return 2.5;*/
        };
        params.sceneManager = scnMgr;
        params.errorMultiplicator = (Real)0.5; // The factor between each LOD-level (error = base
        // Error * errorMultiplicator * level)
        params.skirtFactor = (Real)0.7; // Controls how long the skirts are. The lower the numbe
        // r, the shorter the skirts are. This saves geometry. But if they are too short, cracks might o
        // ccure.
        params.scale = 1; // The displayed volume will be scaled by this factor.
        params.maxScreenSpaceError = 30; // The screen space error controlling when the LOD-levels change.
        params.async = true;
    }

    tree = new LodTree::Tree( params );
    SceneNode * pseudoCamNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    pseudoCamNode->setPosition( 0.0, 0.0, 4105.1 );
    tree->buildTree( pseudoCamNode );
    tree->setMaterial( MaterialManager::getSingleton().getByName( "triplanarReference" ) );


    Entity* ogreEntity = scnMgr->createEntity("ogrehead.mesh");
    SceneNode* ogreNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    ogreNode->attachObject(ogreEntity);
    ogreNode->setPosition( pseudoCamNode->getPosition() );
    ogreNode->setScale( 0.1, 0.1, 0.1 );
}

bool BasicTutorial1::frameRenderingQueued( const FrameEvent & evt )
{
    //return true;
    mTrayManager->frameRendered( evt );
    mAdvancedControls->frameRendered( evt );
    if ( !mTrayManager->isDialogVisible() )
        cMan->frameRendered( evt );
    return true;
}

/*bool BasicTutorial1::frameStarted( const Ogre::FrameEvent & evt )
{
    return true;
}*/

bool BasicTutorial1::frameEnded( const Ogre::FrameEvent & evt )
{
//    mTrayManager->frameRendered( evt );
    //mAdvancedControls->frameRendered( evt );
//    if ( !mTrayManager->isDialogVisible() )
//        cMan->frameRendered( evt );
    return true;
}

bool BasicTutorial1::keyPressed(const KeyboardEvent& evt)
{
    mTrayManager->keyPressed( evt );
    cMan->keyPressed( evt );
    mAdvancedControls->keyPressed( evt );
    if (evt.keysym.sym == SDLK_ESCAPE)
    {
        getRoot()->queueEndRendering();
    }
    return true;
}

bool BasicTutorial1::keyReleased(const KeyboardEvent& evt)
{
    mTrayManager->keyReleased( evt );
    cMan->keyReleased( evt );
    mAdvancedControls->keyReleased( evt );
    return true;
}

bool BasicTutorial1::mouseMoved( const MouseMotionEvent & evt )
{
    if ( mTrayManager->mouseMoved( evt ) )
        return true;
    cMan->mouseMoved( evt );
    mAdvancedControls->mouseMoved( evt );
    return true;
}

bool BasicTutorial1::mousePressed( const MouseButtonEvent & evt )
{
    if ( mTrayManager->mousePressed( evt ) )
        return true;
    if ( evt.button == BUTTON_LEFT )
    {
        cMan->setStyle( CS_FREELOOK );
        mTrayManager->hideCursor();
    }
    cMan->mousePressed( evt );
    mAdvancedControls->mousePressed( evt );
    return true;
}

bool BasicTutorial1::mouseReleased( const MouseButtonEvent & evt )
{
    if ( mTrayManager->mouseReleased( evt ) )
        return true;
    if ( evt.button == BUTTON_LEFT )
    {
        cMan->setStyle( CS_MANUAL );
        mTrayManager->showCursor();
    }
    cMan->mouseReleased( evt );
    mAdvancedControls->mouseReleased( evt );
    return true;
}

int main(int argc, char **argv)
{
    try
    {
    	BasicTutorial1 app;
        app.initApp();
        app.getRoot()->startRendering();
        app.closeApp();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occurred during execution: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

//! [fullsource]
