
#include "air_mesh.h"
#include "BtOgreGP.h"

namespace AirMesh
{

AtmosphereForces::AtmosphereForces()
    : r0( 0.0, 0.0, 0.0 )
{
    viscosityFwd = 0.2;
    viscosityBwd = 0.1;
    viscosityLat = 0.05;

    radius = 100.0;
    height = 100.0;
    groundDensity = 1.0;
}

AtmosphereForces::~AtmosphereForces()
{

}

void AtmosphereForces::forceTorque( const Triangle & tri,
                                    const Ogre::Vector3 & velocity,
                                    Ogre::Vector3 & F, Ogre::Vector3 & P ) const
{
    const Ogre::Real absV = velocity.length();
    if ( absV > 0.0001 )
    {
        const Ogre::Real d = velocity.dotProduct( tri.norm ) / absV;
        const Ogre::Vector3 vNorm = velocity * d;
        const Ogre::Vector3 vTang = velocity - vNorm;
        const Ogre::Real  viscosityNorm = ( (d >= 0.0) ? viscosityFwd : viscosityBwd );
        const Ogre::Vector3 fNorm = vNorm * viscosityNorm;
        const Ogre::Vector3 fTang = vTang * viscosityLat;
        const Ogre::Vector3 f = -(fNorm + fTang) * tri.area;
        const Ogre::Vector3 p = tri.at.crossProduct( f );
        F += f;
        P += p;
    }
}

Ogre::Real AtmosphereForces::density( const Ogre::Vector3 & at ) const
{
    const Ogre::Vector3 dr = at - r0;
    const Ogre::Real h = dr.length() - radius;
    if ( h < 0.0 )
        return groundDensity;
    else if ( h > height )
        return 0.0;
    // Should be exponential. But for simplicity
    // make it quadratic.
    const Ogre::Real dh = (h - height)/height;
    const Ogre::Real ro = groundDensity * dh * dh;
    return ro;
}








Gravity::Gravity()
{
    GM     = 12345.0;
    radius = 100.0;
}

Gravity::~Gravity()
{

}

Ogre::Vector3 Gravity::gravity( const Ogre::Real m, const Ogre::Vector3 & r )
{
    const Ogre::Vector3 dr = r;
    const Ogre::Real d = dr.length();
    // To avoid  troubles if the body is inside.
    if ( d < radius*0.5 )
    {
        const Ogre::Vector3 g = Ogre::Vector3( 0.0, 0.0, 0.0 ); //GM * r / (radius*radius*radius);
        return g;
    }
    const Ogre::Vector3 g = -dr * GM / ( d*d*d );
    return g;
}

Ogre::Vector3 Gravity::gravity( const Ogre::Real m, const Ogre::Vector3 & r0, const Ogre::Vector3 & r )
{
    const Ogre::Vector3 dr = r - r0;
    const Ogre::Real d = dr.length();
    // To avoid  troubles if the body is inside.
    if ( d < radius*0.5 )
    {
        const Ogre::Vector3 g = Ogre::Vector3( 0.0, 0.0, 0.0 ); //GM * r / (radius*radius*radius);
        return g;
    }
    const Ogre::Vector3 g = -dr * GM / ( d*d*d );
    return g;
}








bool AirMesh::airMesh( Ogre::Entity * e, AirMesh & a )
{
    a.tris.clear();

    BtOgre::StaticMeshToShapeConverter c( e );
    const Ogre::int32 indsQty = c.getIndexCount();
    const Ogre::Vector3 * verts = c.getVertices();
    const unsigned int * inds = c.getIndices();
    for ( int i=0; i<indsQty; i+=3 )
    {
        const unsigned int ind0 = inds[i];
        const unsigned int ind1 = inds[i+1];
        const unsigned int ind2 = inds[i+2];
        const Ogre::Vector3 & v0 = verts[ind0];
        const Ogre::Vector3 & v1 = verts[ind1];
        const Ogre::Vector3 & v2 = verts[ind2];
        const Ogre::Vector3 c = (v0 + v1 + v2) * 0.3333333333;
        const Ogre::Vector3 v01 = v1 - v0;
        const Ogre::Vector3 v02 = v2 - v0;
        Ogre::Vector3 n = v01.crossProduct( v02 );
        Triangle tri;
        tri.area = 0.5 * n.normalise();
        tri.norm =  Ogre::Vector3( n.x, n.y, n.z );
        tri.at   =  Ogre::Vector3( c.x, c.y, c.z );
        a.tris.push_back( tri );
    }

    return true;
}

bool AirMesh::airMesh( Ogre::MeshPtr m, AirMesh & a )
{
    a.tris.clear();

    BtOgre::StaticMeshToShapeConverter c;
    c.addMesh( m );
    const Ogre::int32 indsQty = c.getIndexCount();
    const Ogre::Vector3 * verts = c.getVertices();
    const unsigned int * inds = c.getIndices();
    for ( int i=0; i<indsQty; i+=3 )
    {
        const unsigned int ind0 = inds[i];
        const unsigned int ind1 = inds[i+1];
        const unsigned int ind2 = inds[i+2];
        const Ogre::Vector3 & v0 = verts[ind0];
        const Ogre::Vector3 & v1 = verts[ind1];
        const Ogre::Vector3 & v2 = verts[ind2];
        const Ogre::Vector3 c = (v0 + v1 + v2) * 0.3333333333;
        const Ogre::Vector3 v01 = v1 - v0;
        const Ogre::Vector3 v02 = v2 - v0;
        Ogre::Vector3 n = v01.crossProduct( v02 );
        Triangle tri;
        tri.area = 0.5 * n.normalise();
        tri.norm =  Ogre::Vector3( n.x, n.y, n.z );
        tri.at   =  Ogre::Vector3( c.x, c.y, c.z );
        a.tris.push_back( tri );
    }

    return true;
}

AirMesh::AirMesh()
{

}

AirMesh::~AirMesh()
{

}

AirMesh::AirMesh( const AirMesh & inst )
{

}

const AirMesh & AirMesh::operator=( const AirMesh & inst )
{
    if ( this != &inst )
        tris = inst.tris;
    return *this;
}

const Ogre::Vector3 & AirMesh::forceTorque(const AtmosphereForces & f, const Ogre::Vector3 & position,
                                        const Ogre::Vector3 & velocity,
                                        const Ogre::Quaternion & orientation,
                                        Ogre::Vector3 & resF, Ogre::Vector3 & resP ) const
{
    Ogre::Vector3 F( 0.0, 0.0, 0.0 );
    Ogre::Vector3 P( 0.0, 0.0, 0.0 );

    Ogre::Quaternion vq( 0.0, velocity.x, velocity.y, velocity.z );
    vq = orientation.Inverse() * vq * orientation;
    const Ogre::Vector3 v( vq.x, vq.y, vq.z );

    for ( std::vector<Triangle>::const_iterator it=tris.begin();
          it!=tris.end(); it++ )
    {
        const Triangle & tri = *it;
        f.forceTorque( tri, v, F, P );
    }

    Ogre::Quaternion Fq( 0.0, F.x, F.y, F.z );
    Fq = orientation * Fq * orientation.Inverse();
    Ogre::Quaternion Pq( 0.0, P.x, P.y, P.z );
    Pq = orientation * Pq * orientation.Inverse();
    F = Ogre::Vector3( Fq.x, Fq.y, Fq.z );
    P = Ogre::Vector3( Pq.x, Pq.y, Pq.z );
    const Ogre::Real ro = f.density( position );
    F *= ro;
    P *= ro;

    if ( F.length() > 10.0 )
    {
        int i=0;
    }

    resF += F;
    resP += P;
}




}



