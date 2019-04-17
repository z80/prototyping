
#ifndef __PLANET_TEST_H_
#define __PLANET_TEST_H_

#include "planet_base.h"

class PlanetTest: public PlanetBase
{
    URHO3D_OBJECT( PlanetTest, PlanetBase )
public:
    PlanetTest( Context * ctx );
    ~PlanetTest();
};



#endif



