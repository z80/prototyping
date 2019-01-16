
#ifndef __PIVOT_MARKER_H_
#define __PIVOT_MARKER_H_

#include "entity.h"
#include "tech_tree.h"

namespace Osp
{

class Block;

class PivotMarker: public Entity
{
public:
    PivotMarker( Ogre::Real sz = 1.0 );
    ~PivotMarker();

    void setSize( Ogre::Real sz );
public:
    Ogre::Entity * visualEntity;
    ConnectionDesc desc;
};

/**
 * @brief The DesignBlock class
 * This class is just for convenience. It holds a part
 * with all it's pivot points. It douesn't manage it's
 * content. It is DesignConstruction's responsibility
 * to properly init/destroy this class's content.
 */
class DesignBlock
{
public:
    Block * block;
    std::vector<PivotMarker *> markers;

    void setMarkersVisible( bool en );
    bool connectToNearest( std::vector<DesignBlock *> & designs, Ogre::Real & R );

    DesignBlock();
    ~DesignBlock();
    DesignBlock( const DesignBlock & inst );
    const DesignBlock & operator=( const DesignBlock & inst );
};

}


#endif



