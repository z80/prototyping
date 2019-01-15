
#ifndef __PIVOT_MARKER_H_
#define __PIVOT_MARKER_H_

#include "entity.h"

namespace Osp
{

class PivotMarker: public Entity
{
public:
    PivotMarker( Ogre::Real sz = 1.0 );
    ~PivotMarker();

    void setSize( Ogre::Real sz );
public:
    Ogre::Entity * visualEntity;
};

}


#endif



