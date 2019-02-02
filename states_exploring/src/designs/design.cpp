
#include "design.h"
#include "state_manager.h"
#include "config_reader.h"
#include "tinyxml2.h"
#include <sstream>

// Check assembly validity.
#include "lemon/list_graph.h"
#include "lemon/connectivity.h"

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
        blocks  = inst.blocks;
        joints = inst.joints;
    }
    return *this;
}

bool Design::save( const Ogre::String & fname, bool overwrite ) const
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement * root = doc.NewElement( fname.c_str() );
    doc.InsertFirstChild( root );

    // Save blocks.
    tinyxml2::XMLElement * e_blocks = doc.NewElement( "blocks" );
    const size_t partsQty = blocks.size();
    {
        std::ostringstream out;
        out << partsQty;
        e_blocks->SetAttribute( "qty", out.str().c_str() );
    }
    {
        /*std::ostringstream out;
        for ( size_t i=0; i<partsQty; i++ )
        {
            Ogre::String & name = blocks[i];
            out << name << " ";
        }
        e->SetText( out.str().c_str() );*/

        for ( size_t i=0; i<partsQty; i++ )
        {
            const Design::Block & block = blocks[i];
            tinyxml2::XMLElement * e_block = doc.NewElement( block.name.c_str() );
            e_blocks->InsertEndChild( e_block );

            tinyxml2::XMLElement * er = doc.NewElement( "r" );
            {
                std::ostringstream out;
                out << block.r.x << " " << block.r.y << " " << block.r.z;
                er->SetText( out.str().c_str() );
            }
            e_block->InsertEndChild( er );

            tinyxml2::XMLElement * eq = doc.NewElement( "q" );
            {
                std::ostringstream out;
                out << block.q.w << " " << block.q.x << " " << block.q.y << " " << block.q.z;
                eq->SetText( out.str().c_str() );
            }
            e_block->InsertEndChild( eq );
        }

    }
    root->InsertFirstChild( e_blocks );

    // Save connections.
    tinyxml2::XMLElement * e_joints = doc.NewElement( "joints" );
    root->InsertEndChild( e_joints );
    const size_t jointsQty = joints.size();
    {
        std::ostringstream out;
        out << jointsQty;
        e_joints->SetAttribute( "qty", out.str().c_str() );
    }
    for ( size_t i=0; i<jointsQty; i++ )
    {
        const Joint & c = joints[i];
        tinyxml2::XMLElement * e_joint = doc.NewElement( "joint" );
        e_joints->InsertEndChild( e_joint );
        {
            std::ostringstream out;
            out << c.blockA << " " << c.blockB;
            e_joint->SetText( out.str().c_str() );
        }
    }

    tinyxml2::XMLError eResult = doc.SaveFile( fname.c_str() );
    //tinyxml2::XMLCheckResult( eResult );

    return ( eResult == tinyxml2::XML_SUCCESS );
}

bool Design::load( const Ogre::String & fname )
{
    blocks.clear();
    joints.clear();;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.LoadFile( fname.c_str() );
    if ( eResult != tinyxml2::XML_SUCCESS )
        return false;

    tinyxml2::XMLElement * root = doc.FirstChildElement( fname.c_str() );
    if ( !root )
        return false;

    // Load block names.
    tinyxml2::XMLElement * e_blocks = root->FirstChildElement( "blocks" );
    if ( !e_blocks )
        return false;
    size_t qty;
    {
        std::istringstream in( e_blocks->Attribute( "qty" ) );
        in >> qty;
    }
    blocks.reserve( qty );
    std::istringstream in( e_blocks->Value() );
    for ( size_t i=0; i<qty; i++ )
    {
        tinyxml2::XMLElement * e_block = 0;
        if ( i == 0 )
            e_block = e_blocks->FirstChildElement();
        else
            e_block = e_block->NextSiblingElement();
        if ( !e_block )
            return false;

        Design::Block b;
        b.name = e_block->Name();

        // Read position and orientation.
        tinyxml2::XMLElement * er = e_block->FirstChildElement( "r" );
        if ( !er )
            return false;
        {
            std::istringstream in( er->Value() );
            in >> b.r.x >> b.r.y >> b.r.z;
        }
        tinyxml2::XMLElement * eq = e_block->FirstChildElement( "q" );
        if ( !eq )
            return false;
        {
            std::istringstream in( eq->Value() );
            in >> b.q.w >> b.q.x >> b.q.y >> b.q.z;
        }

        blocks.push_back( b );
    }

    // Load connections.
    e_blocks = root->FirstChildElement( "joints" );
    if ( !e_blocks )
        return false;
    {
        std::istringstream in( e_blocks->Attribute( "qty" ) );
        in >> qty;
    }
    joints.reserve( qty );
    for ( size_t i=0; i<qty; i++ )
    {
        Joint joint;
        tinyxml2::XMLElement * e_joint;
        if ( i==0 )
            e_joint = e_blocks->FirstChildElement();
        else
            e_joint = e_joint->NextSiblingElement();
        if ( !e_joint )
            return false;
        {
            std::istringstream in( e_joint->Value() );
            in >> joint.blockA;
            in >> joint.blockB;
        }
        joints.push_back( joint );
    }

    return true;
}

bool Design::valid() const
{
    using namespace lemon;
    typedef Undirector<ListDigraph> Graph;
    ListDigraph g;
    ListDigraph::NodeMap<int> indsMap(g);
    const size_t blocksQty = blocks.size();
    for ( size_t i=0; i<blocksQty; i++ )
    {
        ListDigraph::Node u = g.addNode();
        indsMap[u] = (int)i;
    }
    const size_t jointsQty = joints.size();
    for ( size_t i=0; i<jointsQty; i++ )
    {
        const Joint & c = joints[i];
        ListDigraph::Node a = ListDigraph::nodeFromId(c.blockA);
        ListDigraph::Node b = ListDigraph::nodeFromId(c.blockB);
        /*ListDigraph::Arc  arc =*/ g.addArc(a, b);
    }

    // 2) Count connected components.
    Graph graph(g);
    const int compsQty = countConnectedComponents( graph );

    const bool ok = (compsQty == 1);
    return ok;
}






}



