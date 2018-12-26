
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

Assembly * Assembly::create( Design & design )
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

Ogre::Vector3    Assembly::relR() const
{
    return r;
}

Ogre::Quaternion Assembly::relQ() const
{
    return q;
}

Ogre::Vector3    Assembly::relV() const
{
    return v;
}

Ogre::Vector3    Assembly::relW() const
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
        // Do nothing here as physical world integration
        // is supposed to be done only once.
    }
    else
    {
        // Use Kepler's laws to predict position.
    }
}

void Assembly::setParent( EntityPlanet * planet )
{
    if ( !sceneNode )
        return;

    if ( nearSurface )
    {
        // Compute assembly's pose using part poses.
        computeAssemblyRQVW();

        // Return all parts to assembly.
        const size_t qty = parts.size();
        for ( size_t i=0; i<qty; i++ )
        {
            EntityPart * p = parts[i];
            p->setR( p->assemblyR );
            p->setQ( p->assemblyQ );
            p->setV( Ogre::Vector3( 0.0, 0.0, 0.0 ) );
            p->setW( Ogre::Vector3( 0.0, 0.0, 0.0 ) );
            p->sceneNode->getParent()->removeChild( p->sceneNode );
            sceneNode->addChild( p->sceneNode );

            p->sceneNode->setInheritOrientation( true );
            p->sceneNode->setInheritScale( false );
        }
    }

    sceneNode->setInheritOrientation( false );
    sceneNode->setInheritScale( false );

    Ogre::Vector3    r = absoluteR();
    Ogre::Quaternion q = absoluteQ();
    Ogre::Vector3    v = absoluteV();
    Ogre::Vector3    w = absoluteW();

    if ( parent!=planet )
    {
        sceneNode->getParentSceneNode()->removeChild( sceneNode );
        if ( parent )
        {
            const Ogre::Vector3    parentR = parent->absoluteR();
            const Ogre::Quaternion parentQ = parent->absoluteQ();
            const Ogre::Vector3    parentV = parent->absoluteV();
            // Due to not near surface, "w" is not added up.
            const Ogre::Vector3    parentW = parent->absoluteW();

            // Convert values to global ref. frame.
            r = parentR + r;
            if ( nearSurface )
                q = parentQ * q;
            v = parentV + v;
            if ( nearSurface )
                w = parentW + w;
        }

        if ( planet )
        {
            const Ogre::Vector3    parentR = parent->absoluteR();
            const Ogre::Vector3    parentV = parent->absoluteV();

            // Convert to new parent's ref. frame.
            r = r - parentR;
            v = v - parentV;

            planet->sceneNode->addChild( sceneNode );
        }
        else
        {
            Ogre::SceneManager * scnMgr = StateManager::getSingletonPtr()->getSceneManager();
            scnMgr->getRootSceneNode()->addChild( sceneNode );
        }
    }

    // Update position and movement parameters.
    setR( r );
    setQ( q );
    setV( v );
    setW( w );

    // Update condition variables.
    parent      = planet;
    // Switch to being not near surface.
    nearSurface = false;
}

void Assembly::setParentRf( EntityPlanet * planet )
{
    if ( !sceneNode )
        return;

    if ( nearSurface )
    {
        // Compute assembly's pose using part poses.
        computeAssemblyRQVW();

        // Return all parts to assembly.
        const size_t qty = parts.size();
        for ( size_t i=0; i<qty; i++ )
        {
            EntityPart * p = parts[i];
            p->setR( p->assemblyR );
            p->setQ( p->assemblyQ );
            p->setV( Ogre::Vector3( 0.0, 0.0, 0.0 ) );
            p->setW( Ogre::Vector3( 0.0, 0.0, 0.0 ) );
            p->sceneNode->getParent()->removeChild( p->sceneNode );
            sceneNode->addChild( p->sceneNode );

            p->sceneNode->setInheritOrientation( true );
            p->sceneNode->setInheritScale( false );
        }
    }

    sceneNode->setInheritOrientation( true );
    sceneNode->setInheritScale( false );

    // Take all parts from assembly.
    // 1) Compute their poses and pose derivatives.
    computePartsRQVW();
    // 2) Set actual parent to be appropriate planet.
    const size_t qty = parts.size();
    for ( size_t i=0; i<qty; i++ )
    {
        EntityPart * p = parts[i];
        p->sceneNode->getParent()->removeChild(p->sceneNode );
        planet->sceneNode->addChild( p->sceneNode );
    }

    nearSurface = true;
    parent      = planet;
}

void Assembly::partDestroyed( EntityPart * part )
{
    // Need to split assembly into two here.
}

void Assembly::connectionRemoved( EntityPart * partA, EntityPart * partB )
{
    // Need to split assembly into two here.
}

void Assembly::connectionEstablished( EntityPart * partA, EntityPart * partB )
{
    // Need to merge two assemblies here.
}

void Assembly::computeAssemblyRQVW()
{
    if ( parts.empty() )
        return;

    // Right now for simplicity take the very first part
    // and use it as a reference.
    EntityPart * part = *(parts.begin());
    // partR = assR + assQ * assemblyR * assQ.Inverse()
    // assR  = partR - assQ * assemblyR * assQ.Inverse()
    // partQ = assQ * assemblyQ
    // assQ  = partQ * assemblyQ.Inverse()
    const Ogre::Vector3    partR = part->relR();
    const Ogre::Quaternion partQ = part->relQ();
    const Ogre::Quaternion assQ  = partQ * part->assemblyQ.Inverse();
    const Ogre::Vector3    assW  = part->relW();
    Ogre::Quaternion       rq( 0.0, part->assemblyR.x, part->assemblyR.y, part->assemblyR.z );
    rq = assQ * rq * assQ.Inverse();
    const Ogre::Vector3    rel_r( rq.x, rq.y, rq.z );
    const Ogre::Vector3    assR = partR - rel_r;
    const Ogre::Vector3    assV = part->relV() - assW.crossProduct( rel_r );

    r = assR;
    q = assQ;
    w = assW;
    v = assV;

    if ( sceneNode )
    {
        sceneNode->setPosition( r );
        sceneNode->setOrientation( q );
    }
}

void Assembly::computePartsRQVW()
{
    const size_t qty = parts.size();
    for ( size_t i=0; i<qty; i++ )
    {
        EntityPart * p = parts[i];
        // partQ = assQ * assemblyQ;
        // partR = assR + partQ * assemblyR * partQ.Inverse()
        // partW = assW
        // partV = assV + assW x rel_r;
        const Ogre::Quaternion partQ = q * p->assemblyQ;
        Ogre::Quaternion rq( 0.0, p->assemblyR.x, p->assemblyR.y, p->assemblyR.z );
        rq = partQ * rq * partQ.Inverse();
        Ogre::Vector3 rel_r( rq.x, rq.y, rq.z );
        const Ogre::Vector3 partR = r + rel_r;
        const Ogre::Vector3 partW = w;
        const Ogre::Vector3 partV = v + partW.crossProduct( rel_r );
        p->setR( partR );
        p->setQ( partQ );
        p->setV( partV );
        p->setW( partW );
    }

}


}




