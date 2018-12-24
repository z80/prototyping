
#include "design.h"
#include "state_manager.h"
#include "config_reader.h"

namespace Entity
{

static void saveJoint( const ConnectionJoint & j, std::ofstream & out );
static bool loadJoint( ConnectionJoint & j,       std::ifstream & in );

Design::Design()
{

}

Design::~Design()
{

}

bool Design::save( const Ogre::String & fname, bool overwrite )
{
    std::ofstream out( fname );
    try {
        out << "parts = {" << std::endl;
        const size_t partsQty = parts.size();
        for ( size_t i=0; i<partsQty; i++ )
        {
            const Ogre::String & partName = parts[i];
            out << "\"" << partName << "\"";
            if ( i<(partsQty-1) )
                out << ", ";
        }
        out << "}" << std::endl;

        out << "joints = {" << std::endl;

        out << "}" << std::endl;
    } catch( ... )
    {
        return false;
    }

    return true;
}

bool Design::load( const Ogre::String & fname )
{
    Config::ConfigReader * cr = StateManager::getSingletonPtr()->getConfigReader();
    const bool openOk = cr->openFile( fname.c_str() );
    if ( !openOk )
    {
        const char * err;
        if ( cr->error( &err ) )
            Ogre::LogManager::getSingletonPtr()->logError( err );
        return false;
    }

    return true;
}

bool Design::valid() const
{

    return true;
}



static void saveJoint( const ConnectionJoint & j, std::ofstream & out )
{

}

static bool loadJoint( ConnectionJoint & j, std::ifstream & in )
{

    return false;
}


}



