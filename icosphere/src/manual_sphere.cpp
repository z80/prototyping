
#include "manual_sphere.h"
#include "icosphere.h"
#include "sphere_source.h"

namespace IcoHeightmap
{


class ManualSphere::PD
{
public:
    SceneManager * smgr;
    ManualObject * manual;
    Real r;
    Icosphere      sphere;
    //DumbSphere     subdiv;
    SphereSubdrive subdiv;
    SphereSource ss;



    PD( SceneManager * smgr, Real r );
    ~PD();
};

ManualSphere::PD::PD( SceneManager * smgr, Real r )
{
    this->smgr = smgr;
    this->r    = r;
    manual = smgr->createManualObject();
}

ManualSphere::PD::~PD()
{
    smgr->destroyManualObject( manual );
}


ManualSphere::ManualSphere( SceneManager * smgr, Real r )
{
    pd = new PD( smgr, r );
}

ManualSphere::~ManualSphere()
{
    delete pd;
}

void ManualSphere::generate()
{
    pd->sphere.clear();
    pd->sphere.subdrive( &(pd->subdiv) );
    pd->sphere.applySource( &(pd->ss) );
}

void ManualSphere::fillAndShow( const String & materialName, SceneNode * sceneNode )
{
    ManualObject * m = pd->manual;
    m->clear();

    const int32 fullTrisQty = static_cast<int32>( pd->sphere.tris.size() );
    int n = 0;
    for ( int32 i=0; i<fullTrisQty; i++ )
    {
        const Triangle & tri = pd->sphere.tris[i];
        if ( tri.leaf )
            n += 1;
    }
    const int32 trisQty  = n;
    const int   vertsQty = static_cast<int32>( pd->sphere.verts.size() );
    const Real  r        = pd->r;
    m->estimateIndexCount( trisQty*3 );
    m->estimateVertexCount( vertsQty );

    m->begin( materialName );

    for ( int32 i=0; i<vertsQty; i++ )
    {
        const Vertex & v = pd->sphere.verts[i];
        const Vector3 at = v.at * r;
        m->position( at );
        m->normal( v.norm );
    }
    for ( int32 i=0; i<fullTrisQty; i++ )
    {
        const Triangle & t = pd->sphere.tris[i];
        if ( t.leaf )
        {
            const int32 ind1 = t.vertInds[0];
            const int32 ind2 = t.vertInds[1];
            const int32 ind3 = t.vertInds[2];

            m->triangle( ind2, ind1, ind3 );
        }
    }
    m->end();

    sceneNode->detachObject( m );
    sceneNode->attachObject( m );
}



}


