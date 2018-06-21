
#include "icosphere.h"
#include "sphere_source.h"

using namespace IcoHeightmap;

int main( int argc, char * argv[] )
{
    DumbSphere subdiv( 2 );
    SphereSource ss;
    Icosphere ico;
    ico.subdrive( &subdiv );
    ico.labelMidPoints();
    ico.scaleToSphere();
    ico.applySource( &ss );


    return 0;
}

