
#ifndef __SPHERE_SOURCE_H_
#define __SPHERE_SOURCE_H_

#include "icosphere.h"

namespace IcoHeightmap
{

class DumbSphere: public NeedSubdrive
{
public:
    DumbSphere( int32 n = 5 );
    ~DumbSphere();

    bool subdrive( const Icosphere * s, const Triangle * tri ) const;
public:
    int32 maxLevel;
};

class SphereSubdrive: public NeedSubdrive
{
public:
    SphereSubdrive();
    ~SphereSubdrive();

    void setCameraAt( const Vector3 & camAt );
    bool subdrive( const Icosphere * s, const Triangle * tri ) const;

    int32   maxLevel1, maxLevel2, maxLevel3;
    Real    maxD2, maxD3;
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




