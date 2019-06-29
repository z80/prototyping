
#ifndef __PLANET_SYSTEM_LOADER_H_
#define __PLANET_SYSTEM_LOADER_H_

#include "planet_base.h"
#include "Urho3D/Urho3DAll.h"


namespace Osp
{

class GameData;

class PlanetSystemLoader
{
public:
    static bool load( GameData * gd, Scene * s );
    static bool loadPlanet( const String & name, PlanetBase * parent, const JSONValue & v, GameData * gd, Scene * s );
};

}



#endif


