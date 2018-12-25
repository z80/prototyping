
#include "assembly.h"
#include "state_manager.h"

namespace Entity
{

Assembly::Assembly()
{

}

Assembly::~Assembly()
{

}

static Assembly * Assembly::create( Design & design )
{
    return 0;
}

void Assembly::setR( const Ogre::Vector3 & r )
{
    if ( nearSurface )
        setParent( parent );
    this->r = r;
}

void Assembly::setQ( const Ogre::Quaternion & q )
{
    if ( nearSurface )
        setParent( parent );
    this->q = q;
}

void Assembly::setV( const Ogre::Vector3 & v )
{
    if ( nearSurface )
        setParent( parent );
    this->v = v;
}

void Assembly::setW( const Ogre::Vector3 & w )
{
    if ( nearSurface )
        setParent( parent );
    this->w = w;
}

Ogre::Vector3    Assembly::locR() const
{
    return r;
}

Ogre::Quaternion Assembly::locQ() const
{
    return q;
}

Ogre::Vector3    Assembly::locV() const
{
    return v;
}

Ogre::Vector3    Assembly::locW() const
{
    return w;
}

Ogre::Vector3 Assembly::absoluteR()    const
{
    Ogre::Vector3 r = relR();
    // Parent should always present.
    // But just in case...
    if ( parent )
    {
        const Ogre::Vector3 r_par = parent->absoluteR();
        if ( nearSurface )
        {
            const Ogre::Quaternion q_par = parent->absoluteQ();
            Ogre::Quaternion rq( 0.0, r.x, r.y, r.z );
            rq = q_par * rq * q_par.Inverse();
            r = Ogre::Vector3( rq.x, rq.y, rq.z );
        }
        r = r_par + r;
    }
    return r;
}

Ogre::Quaternion Assembly::absoluteQ() const
{
    Ogre::Quaternion q = relQ();
    if ( parent && nearSurface )
    {
        const Ogre::Quaternion q_parent = parent->absoluteQ();
        q = q_parent * q;
    }
    return q;
}

Ogre::Vector3 Assembly::absoluteV()    const
{
    Ogre::Vector3 v = relV();
    if ( parent )
    {
        const Ogre::Vector3 v_par = parent->absoluteV();
        if ( nearSurface )
        {
            // First convert speed to global ref. frame and add
            // rotational speed in that point.
            const Ogre::Quaternion q_par = parent->absoluteQ();
            Ogre::Quaternion vq( 0.0, v.x, v.y, v.z );
            vq = q_par * vq * q_par.Inverse();
            v = Ogre::Vector3( vq.x, vq.y, vq.z );

            const Ogre::Vector3 at = relR();
            Ogre::Vector3 v_par_at = parent->rotV( at, true );
            v = v_par_at + v;
        }
        v = v_par + v;
    }
    return v;
}

Ogre::Vector3 Assembly::absoluteW()    const
{
    Ogre::Vector3 w = relW();
    if ( nearSurface && parent )
    {
        const Ogre::Vector3 w_parent    = parent->absoluteW();
        const Ogre::Quaternion q_parent = parent->absoluteQ();
        Ogre::Quaternion wq( 0.0, w.x, w.y, w.z );
        wq = q_parent * wq * q_parent.Inverse();
        w = Ogre::Vector3( wq.x, wq.y, wq.z );
        w = w_parent + w;
    }
    return w;
}

void Assembly::integrateDynamics( Ogre::Real t_sec, int timeBoost )
{
    // If in space follow orbital path.
    // Else integrate dynamics.
    if ( nearSurface )
    {
        //for ( int i=0; i<timeBoost; i++ )
        //    StateManager::getSingletonPtr()->getWorld()
        // Do nothing here as physical world integration
        // is supposed to be done only once.
    }
    else
    {
        // Use Kepler's law to predict position.
    }
}

void Assembly::setParent( EntityPlanet * planet )
{
    if ( nearSurface )
    {
        // Return all parts to assembly.
        computeRQVW();

        // Switch to being not near surface.
        nearSurface = false;
    }
}

void Assembly::setParentRf( EntityPlanet * planet )
{

}

void Assembly::partsToWorldRf()
{

}

void Assembly::partsToAssemblyRg()
{

}

void Assembly::partDestroyed( EntityPart * part )
{

}

void Assembly::connectionRemoved( EntityPart * partA, EntityPart * partB )
{

}

void Assembly::connectionEstablished( EntityPart * partA, EntityPart * partB )
{

}

void Assembly::computeRQVW()
{
    if ( !nearSurface )
        return;
    if ( parts.empty() )
        return;

    // Right now for simplicity take the very first part
    // and use it as a reference.
    EntityPart * part = *(parts.begin());
    // partR = assR + assQ * assemblyR * assQ.Inverse()
    // assR = partR - assQ * assemblyR * assQ.Inverse()
    // partQ = assQ * assemblyQ
    // assQ = partQ * assemblyQ.Inverse()
}


}




