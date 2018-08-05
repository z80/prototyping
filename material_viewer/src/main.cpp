#include <Ogre.h>
#include <OgreApplicationContext.h>
#include "OgreInput.h"
#include "OgreRTShaderSystem.h"
#include "OgreCameraMan.h"
#include "OgreTrays.h"
#include "OgreAdvancedRenderControls.h"


#include "ImguiManager.h"

using namespace Ogre;
using namespace OgreBites;

class ImguiExample : public OgreBites::ApplicationContext,
                     public OgreBites::InputListener,
                     public TrayListener
{
public:
    TrayManager            * mTrayManager;
    AdvancedRenderControls * mAdvancedControls;
    CameraMan * cMan;


    ImguiExample() : OgreBites::ApplicationContext("OgreImguiExample")
    {
    }

    bool frameStarted(const Ogre::FrameEvent& evt)
    {
        OgreBites::ApplicationContext::frameStarted(evt);

        Ogre::ImguiManager::getSingleton().newFrame(
            evt.timeSinceLastFrame,
            Ogre::Rect(0, 0, getRenderWindow()->getWidth(), getRenderWindow()->getHeight()));

        ImGui::ShowTestWindow();

        return true;
    }

    void shutdown()
    {
        if ( cMan )
            delete cMan;
        if ( mAdvancedControls )
            delete mAdvancedControls;
        if ( mTrayManager )
            delete mTrayManager;
    }

    void setup()
    {
        OgreBites::ApplicationContext::setup();
        addInputListener(this);
        // To hide cursor.
        //this->setWindowGrab();

        Ogre::ImguiManager::createSingleton();
        addInputListener(Ogre::ImguiManager::getSingletonPtr());

        // get a pointer to the already created root
        Ogre::Root* root = getRoot();
        Ogre::SceneManager* scnMgr = root->createSceneManager();
        Ogre::ImguiManager::getSingleton().init(scnMgr);

        // register our scene with the RTSS
        Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        shadergen->addSceneManager(scnMgr);


        Ogre::Light* light = scnMgr->createLight("MainLight");
        Ogre::SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        lightNode->setPosition(0, 10, 15);
        lightNode->attachObject(light);


        Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
        camNode->setPosition(0, 0, 15);
        camNode->lookAt(Ogre::Vector3(0, 0, -1), Ogre::Node::TS_PARENT);

        Ogre::Camera* cam = scnMgr->createCamera("myCam");
        cam->setNearClipDistance(5); // specific to this sample
        cam->setAutoAspectRatio(true);
        camNode->attachObject(cam);
        getRenderWindow()->addViewport(cam);

        Ogre::Entity* ent = scnMgr->createEntity("Sinbad.mesh");
        Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(ent);


        // *************************************************
        this->locateResources();
        this->loadResources();

        // Cursor controls and tray management.
        Ogre::RenderWindow * wnd = getRenderWindow();
        mTrayManager = new TrayManager( "my_tray", wnd, this );
        //mTrayManager->showCursor();
        mTrayManager->showFrameStats( TL_BOTTOMLEFT );
        //mTrayManager->showAll();
        //mTrayManager->showTrays();

        OverlaySystem * os = this->getOverlaySystem();
        scnMgr->addRenderQueueListener( os );

        mAdvancedControls = new AdvancedRenderControls( mTrayManager, cam );

        // Convenience camera control
        cMan = new CameraMan( camNode );
        cMan->setStyle( CS_MANUAL );

        scnMgr->setSkyBox( true, "Examples/CloudyNoonSkyBox" );
    }

    bool keyPressed(const OgreBites::KeyboardEvent& evt)
    {
        if (evt.keysym.sym == 27)
        {
            getRoot()->queueEndRendering();
        }
        return true;
    }

    bool mousePressed( const MouseButtonEvent & evt )
    {
        if ( mTrayManager->mousePressed( evt ) )
            return true;
        if ( evt.button == BUTTON_LEFT )
        {
            cMan->setStyle( CS_ORBIT );
            mTrayManager->hideCursor();
        }
        cMan->mousePressed( evt );
        mAdvancedControls->mousePressed( evt );
        return true;
    }

    bool mouseReleased( const MouseButtonEvent & evt )
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

    bool mouseMoved( const MouseMotionEvent & evt )
    {
        if ( mTrayManager->mouseMoved( evt ) )
            return true;
        cMan->mouseMoved( evt );
        mAdvancedControls->mouseMoved( evt );
        return true;
    }

};


int main(int argc, char *argv[])
{
    ImguiExample app;
    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();

    return 0;
}
