
#ifndef __PART_DESC_H_
#define __PART_DESC_H_

#include "Ogre.h"
#include "config_reader.h"

namespace Desc
{

struct ConnectionPoint
{
    /// Connection point position in local ref. frame.
    Ogre::Vector3 at;
    /// Connection point "direction"
    /// Connected objects are to have opposite directions.
    Ogre::Vector3 dir;
    /// Connection name (Not sure if I need that though...)
    std::string   name;
};


/** Collision object might be one of Bullet supported shapes.
Sphere,
Box,
Cone,
Mesh
**/

struct CollisionObject
{
    // Collision object type.
    std::string type;
    // Params which may accompany this collision object.
    std::vector<Ogre::Real> params;
    // If it's a mesh it needs a file name.
    std::string fileName;
};

struct Piece
{
    std::string     visualMesh;
    std::string     airMesh;
    CollisionObject collisionObj;
};

struct Parameter
{
    std::string name;
    Ogre::Real  value;
    Parameter( const std::string & stri, Ogre::Real v )
    {
        name  = stri;
        value = v;
    }

    static struct _Lt
    {
        bool operator()( const Parameter & a, const Parameter & b )
        {
            const bool res = (a.name < b.name);
            return res;
        }
        bool operator()( const Parameter & a, const std::string & b )
        {
            const bool res = (a.name < b);
            return res;
        }
        bool operator()( const std::string & a, const Parameter & b )
        {
            const bool res = (a < b.name);
            return res;
        }
    } Lt;
};

struct Sound
{
    std::string name;
    std::string fileName;

    Sound( const std::string n, const std::string & fn )
    {
        name     = n;
        fileName = fn;
    }

    static struct _Lt
    {
        bool operator()( const Sound & a, const Sound & b )
        {
            const bool res = (a.name < b.name);
            return res;
        }
        bool operator()( const Sound & a, const std::string & b )
        {
            const bool res = (a.name < b);
            return res;
        }
        bool operator()( const std::string & a, const Sound & b )
        {
            const bool res = (a < b.name);
            return res;
        }
    } Lt;
};

class PartDesc
{
public:
    PartDesc();
    ~PartDesc();
    PartDesc( const PartDesc & inst );
    const PartDesc & operator=( const PartDesc & inst );

    void addParam( const std::string & name, Ogre::Real value );
    bool param( const std::string & name, Ogre::Real & value );

    void addSound( const std::string & name, const std::string & fileName );

    void finalize();

    std::vector<Piece>           parts;
    std::vector<ConnectionPoint> connections;
    std::vector<Sound>           sounds;
    std::vector<Parameter>       params;
    Ogre::Real    mass;
    Ogre::Vector3 inertia;

    std::string name;
    std::string desc;
    std::string group;
    std::string icon;
    int         level;
};

void luaopen_partdesc( lua_State * L );


}

#endif





