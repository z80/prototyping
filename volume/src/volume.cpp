
#include "volume.h"
#include "sphere_source.h"

Volume2::Volume2( SceneManager * smgr )
{
    this->smgr = smgr;
    volume     = 0;
    volumeNode = 0;

    create();
}

Volume2::~Volume2()
{
    destroy();
}

void Volume2::create()
{
    if ( volume )
        destroy();

    SphereSource ss( 5.0, Vector3( 0.0, 0.0, 0.0 ) );


    ChunkParameters parameters;
    parameters.sceneManager = smgr; // The SceneManager to use
    parameters.src = &ss; // The just created density source.
    parameters.baseError = (Real)0.18; // The error of the highest LOD-level
    parameters.errorMultiplicator = (Real)0.9; // The factor between each LOD-level (error = base
    // Error * errorMultiplicator * level)
    parameters.skirtFactor = (Real)0.7; // Controls how long the skirts are. The lower the numbe
    // r, the shorter the skirts are. This saves geometry. But if they are too short, cracks might o
    // ccure.
    parameters.scale = 10; // The displayed volume will be scaled by this factor.
    parameters.maxScreenSpaceError = 3; // The screen space error controlling when the LOD-levels change.


    volumeNode = smgr->getRootSceneNode()->createChildSceneNode();

    volume = OGRE_NEW Chunk();
    volume->load( volumeNode, Vector3(-256), Vector3(256), 2, &parameters );

    MaterialPtr mh = MaterialManager::getSingleton().getByName( "Examples/CloudySky" );
    MaterialPtr ml = MaterialManager::getSingleton().getByName( "Examples/CloudySky" );
    volume->setMaterial( mh );
    volume->setMaterialOfLevel( 0, ml );
    volume->setMaterialOfLevel( 1, ml );
    volume->setMaterialOfLevel( 2, ml );
}

void Volume2::destroy()
{
    if ( volume )
    {
        OGRE_DELETE volume;
        volume = 0;
    }
    if ( volumeNode )
    {
        volumeNode->detachAllObjects();
        volumeNode = 0;
    }
}





