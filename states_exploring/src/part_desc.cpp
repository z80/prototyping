
#include "part_desc.h"
#include <vector>
#include <algorithm>

namespace Desc
{

PartDesc::PartDesc()
{
    level = 0;
}

PartDesc::~PartDesc()
{

}

PartDesc::PartDesc( const PartDesc & inst )
{
    *this = inst;
}

const PartDesc & PartDesc::operator=( const PartDesc & inst )
{
    if ( this != &inst )
    {
        parts       = inst.parts;
        connections = inst.connections;
        params      = inst.params;
        mass        = inst.mass;
        inertia     = inst.inertia;

        name        = inst.name;
        desc        = inst.desc;
        group       = inst.group;
        icon        = inst.icon;
        level       = inst.level;

    }
    return *this;
}

void PartDesc::setParam( const std::string & name, Ogre::Real value )
{
    params.push_back( Parameter( name, value ) );
    std::push_heap( params.begin(), params.end(), Parameter::Lt );
}

void PartDesc::finalize()
{
    std::sort_heap( params.begin(), params.end(), Parameter::Lt );
}

bool PartDesc::param( const std::string & name, Ogre::Real & value )
{
    std::vector<Parameter>::const_iterator it =
            std::lower_bound( params.begin(), params.end(), name, Parameter::Lt );
    if ( it == params.end() )
        return false;
    value = (*it).value;
    return true;
}








}



