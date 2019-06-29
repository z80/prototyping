
#include "planet_system_loader.h"
#include "planet_cs.h"

namespace Osp
{

bool PlanetSystemLoader::load( GameData * gd, Scene * s )
{
    Context * ctx = s->GetContext();
    URHO3D_LOGINFO( "Loading planets hierarchy" );
    JSONFile json( ctx );
    FileSystem * fs = ctx->GetSubsystem<FileSystem>();
    const String stri = fs->GetProgramDir() + "Data/Planets/planets.json";
    const bool loaded = json.LoadFile( stri );
    if ( !loaded )
        URHO3D_LOGINFO( "Failed to load JSON file" );
    const JSONValue & root = json.GetRoot();
    if ( !root.IsObject() )
        URHO3D_LOGINFO( "JSON file doesn't contain proper structure" );

    for ( ConstJSONObjectIterator it=root.Begin(); it!=root.End(); it++ )
    {
        const String & name = it->first_;
        const JSONValue & subitems = it->second_;
        const bool ok = loadPlanet( name, nullptr, subitems, gd, s );
        if ( !ok )
            return false;
    }

    return true;
}

bool PlanetSystemLoader::loadPlanet( const String & name, PlanetBase * parent, const JSONValue & v, GameData * gd, Scene * s )
{
    // First try open planet config file.
    // If it can be open create planet and feed in config file name.
    {
        Context * ctx = s->GetContext();
        URHO3D_LOGINFO( "Loading planet " + name );
        JSONFile json( ctx );
        FileSystem * fs = ctx->GetSubsystem<FileSystem>();
        const String stri = fs->GetProgramDir() + "Data/Planets/" + name + ".json";
        const bool loaded = json.LoadFile( stri );
        if ( !loaded )
        {
            URHO3D_LOGINFO( "Failed to load JSON file" );
            return false;
        }
        const JSONValue & root = json.GetRoot();
        if ( !root.IsObject() )
        {
            URHO3D_LOGINFO( "JSON file doesn't contain proper structure" );
            return false;
        }
    }
    // If parent exists create it as a child.
    // Alternatively create child of Scene.
    Node * n;
    if ( parent )
    {
        Node * pn = parent->GetNode();
        n = pn->CreateChild( name + "Node" );
    }
    else
        n = s->CreateChild( name + "Node" );

    PlanetCs * cs = n->CreateComponent<PlanetCs>();
    cs->setup( "Data/Planets/" + name + ".json" );
    gd->planets.Push( cs );

    // Recursively parse children.
    for ( ConstJSONObjectIterator it=v.Begin(); it!=v.End(); it++ )
    {
        const String & childName = it->first_;
        const JSONValue & subitems = it->second_;
        const bool ok = loadPlanet( childName, cs, subitems, gd, s );
        if ( !ok )
            return false;
    }

    return true;
}


}

