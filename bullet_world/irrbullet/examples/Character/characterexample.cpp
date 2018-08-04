// Copyright (C) 2014- Danyal Zia
// Copyright (C) 2009-2013 Josiah Hartzell (Skyreign Software)
// This file is part of the "irrBullet" Bullet physics wrapper.
// For conditions of distribution and use, see copyright notice in irrBullet.h
// The above copyright notice and its accompanying information must remain here.

#include <irrlicht.h>
#include <irrBullet.h>
#include "characterexample.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;
using namespace io;
using namespace std;

CCharacterExample::CCharacterExample() : DirZ(0.0f), DirX(0.0f)
{
}


bool CCharacterExample::OnEvent(const SEvent& event)
{
	if (!device)
		return false;

    switch(event.EventType)
    {
        case EET_MOUSE_INPUT_EVENT:
        {
            if(event.MouseInput.Event==EMIE_LMOUSE_PRESSED_DOWN)
            {
                /*shootCube(vector3df(2,2,2), 1);
                return true;*/
            }

            else
            if(event.MouseInput.Event==EMIE_RMOUSE_PRESSED_DOWN)
            {
                shootSphere(vector3df(0.2,0.2,0.2), 0.2);
                return true;
            }
        }
        break;

        case EET_KEY_INPUT_EVENT:
        {
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

            if(event.KeyInput.Key == KEY_KEY_P && event.KeyInput.PressedDown == false)
            {
                world->pauseSimulation(!world->simulationPaused());
                return true;
            }

            else
            if(event.KeyInput.Key == KEY_KEY_R && event.KeyInput.PressedDown == false)
            {
                while(world->getNumCollisionObjects() > 0)
                {
                    world->removeCollisionObject(world->getCollisionObjectByIndex(0));
                }
                createGround();
                createBoxes();
                return true;
            }
        }
        break;
        default:
            break;
    }
    return false;
}


void CCharacterExample::runExample()
{
    for(s32 i=0; i<KEY_KEY_CODES_COUNT; i++)
        KeyIsDown[i] = false;

    debugDraw = true;
    drawProperties = true;
    drawWireFrame = false;

    rows = 10;
	columns = 10;

	device.reset(
        createDevice( video::EDT_OPENGL, dimension2d<u32>(640, 480), 16,
            false, false, false, this));


    device->setWindowCaption(L"irrBullet Character Example");
    device->setResizable(true);
    device->getFileSystem()->addFileArchive("../../media/");


    device->getSceneManager()->addLightSceneNode(0, vector3df(20, 40, -50), SColorf(1.0f, 1.0f, 1.0f), 4000.0f);


    // Create irrBullet World 
    world.reset(createIrrBulletWorld(device, true, debugDraw));

	world->setDebugMode(irrPhysicsDebugMode::EPDM_DrawAabb |
		irrPhysicsDebugMode::EPDM_DrawContactPoints);

    world->setGravity(vector3df(0,-10,0));
    world->setPropertiesTextPosition(EDPT_POSITION::EDPT_TOP_RIGHT);

    camera = device->getSceneManager()->addCameraSceneNodeMaya();
	camera->setPosition(vector3df(50,15,200));
	camera->bindTargetAndRotation(true);

	createGround();
	createBoxes();

	auto character = std::make_shared<IKinematicCharacterController>(world.get(), 20);
    character->setJumpForce(30);

	IAnimatedMeshSceneNode* sydney = device->getSceneManager()->addAnimatedMeshSceneNode(device->getSceneManager()->getMesh("sydney.md2"));
	sydney->setScale(vector3df(0.5,0.5,0.5));
	sydney->getMaterial(0).setTexture(0, device->getVideoDriver()->getTexture("sydney.bmp"));
	sydney->setMD2Animation(scene::EMAT_STAND);

    // Set our delta time and time stamp
    u32 TimeStamp = device->getTimer()->getTime();
    u32 DeltaTime = 0;
    EMD2_ANIMATION_TYPE animation = EMAT_STAND;
    EMD2_ANIMATION_TYPE newAnimation = EMAT_RUN;
    bool jump = false;
    while(device->run())
    {
        device->getVideoDriver()->beginScene(true, true, SColor(255,100,101,140));

        DeltaTime = device->getTimer()->getTime() - TimeStamp;
		TimeStamp = device->getTimer()->getTime();


		sydney->setPosition(character->getWorldTransform().getTranslation());
        vector3df dir = camera->getAbsolutePosition()-sydney->getAbsolutePosition();
        dir.normalize();
        dir.Y = 0;
        dir *= -1;

        character->setWalkDirection(dir*0.3f);

        if(IsKeyDown(KEY_KEY_W))
        {
            DirZ = -1.0f;
        }

        else if(IsKeyDown(KEY_KEY_S))
        {
            DirZ = 1.0f;
        }

        else
        {
            DirZ = 0.0f;
        }

        if(IsKeyDown(KEY_KEY_A))
        {
            DirX = 1.0f;
        }

        else if(IsKeyDown(KEY_KEY_D))
        {
            DirX = -1.0f;
        }

        else
        {
            DirX = 0.0f;
        }

        if(IsKeyDown(KEY_SPACE))
        {
            character->jump();
            jump = true;
        }

        if(DirZ != 0 || DirX != 0)
            newAnimation = EMAT_RUN;

        else
            newAnimation = EMAT_STAND;

        if(!character->isOnGround())
        {
            newAnimation = jump ? EMAT_JUMP:EMAT_CROUCH_STAND;
        }

        else
            jump = false;


        if(animation != newAnimation)
        {
            animation = newAnimation;
            sydney->setMD2Animation(animation);
        }


		irr::core::vector3df pos = character->getWorldTransform().getTranslation();
		sydney->setPosition(pos);

        vector3df rot(0, camera->getRotation().Y-90.0f, 0);
        sydney->setRotation(rot);

        vector3df direction(DirX, 0.0f, DirZ);

        irr::core::matrix4 m;

        m.setRotationDegrees(vector3df(0, camera->getRotation().Y-180.0f, 0));

        m.transformVect(direction);

        character->setWalkDirection(direction*0.3f);

        camera->setTarget(sydney->getPosition());

		// Step the simulation with our delta time
        world->stepSimulation(DeltaTime*0.001f, 120);
        world->debugDrawWorld(debugDraw);

        // This call will draw the technical properties of the physics simulation
        // to the GUI environment.
        world->debugDrawProperties(true);


        device->getSceneManager()->drawAll();
        device->getGUIEnvironment()->drawAll();

        device->getVideoDriver()->endScene();
    }
}


void CCharacterExample::createBoxes()
{
    for(u32 j=0; j < columns; j++)
    {
        for(u32 i=0; i < rows; i++)
        {
            addCube(vector3df(3*j, 0+3*i+3, 0), vector3df(10,10,10), 3);
        }
    }
}


void CCharacterExample::createGround()
{
	auto Node = device->getSceneManager()->addCubeSceneNode(1.0);
	Node->setScale(vector3df(300,3,300)); // 400, 3, 400
	Node->setPosition(vector3df(20,0,10));
	Node->setMaterialFlag(video::EMF_LIGHTING, true);
	Node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	Node->setMaterialTexture(0, device->getVideoDriver()->getTexture("rockwall.jpg"));

    if(drawWireFrame)
        Node->setMaterialFlag(EMF_WIREFRAME, true);

	auto shape = new IBoxShape(Node, 0, false);

	auto body = world->addRigidBody(shape);
}


CCharacterExample::~CCharacterExample()
{
}
