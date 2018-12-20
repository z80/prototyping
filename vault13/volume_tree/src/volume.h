
#ifndef __VOLUME_H_
#define __VOLUME_H_

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreVolumeChunk.h"
#include "OgreVolumeCSGSource.h"
#include "OgreVolumeCacheSource.h"
#include "OgreVolumeTextureSource.h"

using namespace Ogre;
using namespace Volume;

class Volume2
{
public:
    Volume2( SceneManager * smgr );
    ~Volume2();

    void create();
    void destroy();

private:
    SceneManager * smgr;
    Chunk        * volume;
    SceneNode    * volumeNode;
};





#endif




