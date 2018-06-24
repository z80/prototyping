
#include "icosphere_asynch.h"


namespace IcoHeightmap
{

IcosphereAsynch::IcosphereAsynch()
{
    initWorkQueue();
}

IcosphereAsynch::~IcosphereAsynch()
{
    // Root might already be shutdown.
    if ( workQueue && Root::getSingletonPtr() )
    {
        workQueue->removeRequestHandler( workQueueChannel,  this );
        workQueue->removeResponseHandler( workQueueChannel, this );
    }
}

void IcosphereAsynch::handleAsynchResults( const Vector3 & camAt )
{
    if ( isBusy )
    {
        workQueue->processResponses();
    }
    else
    {
        const bool needRebuild = rebuild->rebuild( camAt );
        if ( needRebuild )
        {
            isBusy = true;
            const Real r = manualSphere->radius();
            const Vector3 at = camAt / r;
            this->subdrive->at = at;
            Request req;
            req.source   = this->source;
            req.sphere   = this->sphere;
            req.subdrive = this->subdrive;
            Ogre::Any any( req );
            workQueue->addRequest( workQueueChannel, 0, any );
        }
    }
}

WorkQueue::Response * IcosphereAsynch::handleRequest( const WorkQueue::Request * req, const WorkQueue * srcQ )
{
    Request s = any_cast<Request>( req->getData() );
    s.sphere->clear();
    s.sphere->subdrive( s.subdrive );
    s.sphere->applySource( s.source );

    WorkQueue::Response * resp = OGRE_NEW WorkQueue::Response(req, true, Any());
    return resp;
}

void IcosphereAsynch::handleResponse( const WorkQueue::Response * res, const WorkQueue * srcQ )
{
    if ( (!res) || (!res->succeeded()) )
        return;

    manualSphere->update();
    isBusy = false;
}

void IcosphereAsynch::initWorkQueue(void)
{
    workQueue = Root::getSingleton().getWorkQueue();
    workQueueChannel = workQueue->getChannel( "Ogre/IcoHeightmap" );
    mWQ->addResponseHandler( workQueueChannel, this );
    mWQ->addRequestHandler(  workQueueChannel, this );

    isBusy = false;
}






}



