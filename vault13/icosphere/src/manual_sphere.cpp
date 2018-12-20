
#include "manual_sphere.h"
#include "icosphere.h"
#include "sphere_source.h"
#include "icosphere_asynch.h"

namespace IcoHeightmap
{


class ManualSphere::PD
{
public:
    SceneManager * smgr;
    SceneNode    * sceneNode;
    ManualObject * manual;
    String         materialName;
    Real r;
    Icosphere      sphere;
    //DumbSphere     subdiv;
    SphereSubdrive subdiv;
    SphereRebuild  rebuild;
    SphereSource   ss,
                   ssHeight;

    RequestState st;

    IcosphereAsynch * asynch();


    PD( ManualSphere * ms, SceneManager * smgr, Real r );
    ~PD();
};

IcosphereAsynch * ManualSphere::PD::asynch()
{
    static IcosphereAsynch a;
    return &a;
}

ManualSphere::PD::PD( ManualSphere * ms, SceneManager * smgr, Real r )
{
    this->smgr = smgr;
    this->r    = r;
    sceneNode = 0;
    manual = smgr->createManualObject();

    st.isBusy       = false;
    st.manualSphere = ms;
    st.rebuild      = &rebuild;
    st.source       = &ss;
    st.sphere       = &sphere;
    st.subdrive     = &subdiv;
}

ManualSphere::PD::~PD()
{
    if ( Root::getSingletonPtr() )
    {
        while ( st.isBusy )
            OGRE_THREAD_SLEEP( 1 );
        smgr->destroyManualObject( manual );
    }
}


ManualSphere::ManualSphere( SceneManager * smgr, Real r )
{
    pd = new PD( this, smgr, r );
}

ManualSphere::~ManualSphere()
{
    delete pd;
}

void ManualSphere::setSceneNode( SceneNode * sceneNode )
{
    pd->sceneNode = sceneNode;
}

void ManualSphere::setMaterialName( const String & name )
{
    pd->materialName = name;
}

void ManualSphere::setCameraPosition( const Vector3 & at )
{
    const Real r = pd->r;
    const Vector3 camAt = at / r;
    pd->asynch()->handleAsynchResults( &(pd->st), camAt );
}

/*void ManualSphere::generate()
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

            m->triangle( ind1, ind2, ind3 );
        }
    }
    m->end();

    sceneNode->detachObject( m );
    sceneNode->attachObject( m );
}*/

Real ManualSphere::localHeight( const Vector3 & at ) const
{
    Vector3 v = at;
    v.normalise();
    const Real dh = pd->ssHeight.dh( v );
    const Real d  = (1.0 + dh) * pd->r;
    return d;
}

Real ManualSphere::radius() const
{
    const Real r = pd->r;
    return r;
}

void ManualSphere::update()
{
    ManualObject * m = pd->manual;

    const int32 fullTrisQty = static_cast<int32>( pd->sphere.tris.size() );
    int n = 0;
    for ( int32 i=0; i<fullTrisQty; i++ )
    {
        const Triangle & tri = pd->sphere.tris[i];
        if ( tri.leaf )
            n += 1;
    }
    const int32 trisQty  = n;
    const int32 indsQty  = trisQty * 3;
    const int   vertsQty = static_cast<int32>( pd->sphere.verts.size() );
    const Real  r        = pd->r;
    //const size_t currentIndexQty = m->getCurrentIndexCount();
    //const size_t currentVertQty  = m->getCurrentVertexCount();
    //if ( ( indexQty != currentIndexQty ) || ( vertsQty != currentVertQty ) )

    m->clear();
    m->estimateIndexCount( indsQty );
    m->estimateVertexCount( vertsQty );

    m->begin( pd->materialName );

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

            m->triangle( ind1, ind2, ind3 );
        }
    }
    m->end();

    pd->sceneNode->detachObject( m );
    pd->sceneNode->attachObject( m );
}


}


