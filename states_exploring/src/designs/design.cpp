
#include "design.h"
#include "state_manager.h"
#include "config_reader.h"
#include "tinyxml2.h"
#include <sstream>

namespace Osp
{

Design::Design()
{

}

Design::~Design()
{

}

Design::Design( const Design & inst )
{
    *this = inst;
}

const Design & Design::operator=( const Design & inst )
{
    if ( this != &inst )
    {
        parts  = inst.parts;
        joints = inst.joints;
    }
    return *this;
}

bool Design::save( const Ogre::String & fname, bool overwrite )
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement * root = doc.NewElement( fname.c_str() );
    doc.InsertFirstChild( root );

    // Save blocks.
    tinyxml2::XMLElement * e = doc.NewElement( "blocks" );
    const size_t partsQty = parts.size();
    {
        std::ostringstream out;
        out << partsQty;
        e->SetAttribute( "qty", out.str().c_str() );
    }
    {
        std::ostringstream out;
        for ( size_t i=0; i<partsQty; i++ )
        {
            Ogre::String & name = parts[i];
            out << name << " ";
        }
        e->SetText( out.str().c_str() );
    }
    root->InsertFirstChild( e );

    // Save connections.
    e = doc.NewElement( "joints" );
    root->InsertEndChild( e );
    const size_t jointsQty = joints.size();
    {
        std::ostringstream out;
        out << jointsQty;
        e->SetAttribute( "qty", out.str().c_str() );
    }
    for ( size_t i=0; i<jointsQty; i++ )
    {
        const Connection & c = joints[i];
        tinyxml2::XMLElement * ee = doc.NewElement( "joint" );
        e->InsertEndChild( ee );
        {
            std::ostringstream out;
            out << c.partA << " " << c.partB;
            ee->SetText( out.str().c_str() );
        }
        tinyxml2::XMLElement * eee = doc.NewElement( "r" );
        ee->InsertEndChild( eee );
        {
            std::ostringstream out;
            out << c.r.x << " " << c.r.y << " " << c.r.z;
            eee->SetText( out.str().c_str() );
        }
        eee = doc.NewElement( "q" );
        ee->InsertEndChild( eee );
        {
            std::ostringstream out;
            out << c.q.w << " " << c.q.x << " " << c.q.y << " " << c.q.z;
            eee->SetText( out.str().c_str() );
        }
    }

    std::ofstream out( fname );
    tinyxml2::XMLError eResult = doc.SaveFile( fname.c_str() );
    //tinyxml2::XMLCheckResult( eResult );

    return ( eResult == tinyxml2::XML_SUCCESS );
}

bool Design::load( const Ogre::String & fname )
{
    parts.clear();
    joints.clear();;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.LoadFile( fname.c_str() );
    if ( eResult != tinyxml2::XML_SUCCESS )
        return false;

    tinyxml2::XMLElement * root = doc.FirstChildElement( fname.c_str() );
    if ( !root )
        return false;

    // Load block names.
    tinyxml2::XMLElement * e = root->FirstChildElement( "blocks" );
    if ( !e )
        return false;
    size_t qty;
    {
        std::istringstream in( e->Attribute( "qty" ) );
        in >> qty;
    }
    parts.reserve( qty );
    std::istringstream in( e->Value() );
    for ( size_t i=0; i<qty; i++ )
    {
        std::string name;
        in >> name;
        parts.push_back( name );
    }

    // Load connections.
    e = root->FirstChildElement( "joints" );
    if ( !e )
        return false;
    {
        std::istringstream in( e->Attribute( "qty" ) );
        in >> qty;
    }
    joints.reserve( qty );
    for ( size_t i=0; i<qty; i++ )
    {
        Connection c;
        tinyxml2::XMLElement * ee;
        if ( i==0 )
            ee = e->FirstChildElement();
        else
            ee = ee->NextSiblingElement();
        if ( !ee )
            return false;
        {
            std::istringstream in( ee->Value() );
            in >> c.partA;
            in >> c.partB;
            tinyxml2::XMLElement * er = ee->FirstChildElement( "r" );
            if ( !er )
                return false;
            {
                std::istringstream in( er->Value() );
                in >> c.r.x >> c.r.y >> c.r.z;
            }
            tinyxml2::XMLElement * eq = ee->FirstChildElement( "q" );
            if ( !eq )
                return false;
            {
                std::istringstream in( eq->Value() );
                in >> c.q.w >> c.q.x >> c.q.y >> c.q.z;
            }
        }
    }

    return true;
}

bool Design::valid() const
{

    return true;
}






}



