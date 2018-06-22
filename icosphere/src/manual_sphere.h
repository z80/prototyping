
#ifndef __MANUAL_SPHERE_H_
#define __MANUAL_SPHERE_H_

#include "Ogre.h"

using namespace Ogre;

namespace IcoHeightmap
{

class ManualSphere
{
public:
    ManualSphere( SceneManager * smgr, Real r = 10240.0 );
    ~ManualSphere();

    void generate();
    void fillAndShow( const String & materialName, SceneNode * sceneNode );
    Real localHeight( const Vector3 & at ) const;

private:
    class PD;
    PD * pd;
};

}


#endif



