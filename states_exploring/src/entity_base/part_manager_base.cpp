
#include "part_manager_base.h"
#include "entity_part.h"

namespace Entity
{

PartManagerBase::PartManagerBase()
{

}

PartManagerBase::~PartManagerBase()
{
    destroyAllParts();
}

void PartManagerBase::remove( EntityPart * part )
{
    size_t qty = parts.size();
    for ( size_t i=0; i<qty; i++ )
    {
        EntityPart * p = parts[i];
        if ( p == part )
        {
            const size_t qty_1 = qty-1;
            if ( i < (qty_1) )
                parts[i] = parts[qty_1];
            parts.resize( qty_1 );
            delete part;
            break;
        }
    }
}

void PartManagerBase::destroyAllParts()
{
    size_t qty = parts.size();
    for ( size_t i=0; i<qty; i++ )
    {
        EntityPart * p = parts[0];
        delete p;
    }
}

}

