
#ifndef __SPHERE_SOURCE_H_
#define __SPHERE_SOURCE_H_

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreVolumeChunk.h"
#include "OgreVolumeCSGSource.h"
#include "OgreVolumeCacheSource.h"
#include "OgreVolumeTextureSource.h"

using namespace Ogre;
using namespace Volume;

class SphereSource: public Source
{
protected:
    const Real mR;
    const Vector3 mCenter;
public:

    SphereSource(const Real r, const Vector3 &center);
    virtual Vector4 getValueAndGradient(const Vector3 &position) const;
    virtual Real getValue(const Vector3 &position) const;
};






#endif
