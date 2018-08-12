
#include "entity_factory.h"
#include "entity_world.h"
#include "entity_part.h"
#include "entity_planet.h"

namespace Entity
{

static EntityFactory g_ef;
template<> EntityFactory * Ogre::Singleton<EntityFactory>::msSingleton = 0;

class EntityFactory::PD
{
public:
    EntityWorld  * world();
    EntityPart   * cube();
    EntityPart   * plane();
    //EntityPlanet * spherePlanet();

    PD();
    ~PD();
};



EntityFactory::EntityFactory()
{

}

EntityFactory::~EntityFactory()
{

}

Entity * EntityFactory::entityPart( const Ogre::String & name,
                                    const Ogre::Vector3 & at,
                                    const Ogre::Quaternion & q,
                                    Ogre::SceneNode * parentNode )
{
    return 0;
}






}



