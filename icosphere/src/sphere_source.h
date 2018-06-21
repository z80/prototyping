
#ifndef __SPHERE_SOURCE_H_
#define __SPHERE_SOURCE_H_

#include "icosphere.h"

namespace IcoHeightmap
{

class DumbSphere: public NeedSubdrive
{
public:
    DumbSphere( int32 n = 1 );
    ~DumbSphere();

    bool subdrive( const Icosphere * s, const Triangle * tri ) const;
public:
    int32 maxLevel;
};

class SphereSubdrive: public NeedSubdrive
{
public:
    SphereSubdrive( int32 level1 = 3, int32 level2 = 7, Real d = 0.05 );
    ~SphereSubdrive();

    void setCameraAt( const Vector3 & at );
    bool subdrive( const Icosphere * s, const Triangle * tri ) const;

    int32   maxLevel1, maxLevel2;
    Real    maxD;
    Vector3 camAt;
};


class SphereSource: public Source
{
public:
    SphereSource();
    ~SphereSource();

    Real dh( const Vector3 & at ) const;

private:
    class PD;
    PD * pd;
};

}


#endif




