
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

void IcosphereAsynch::handleAsynchResults( RequestState * st, const Vector3 & camAt )
{
    if ( st->isBusy )
    {
        workQueue->processResponses();
    }
    else
    {
        const bool needRebuild = st->rebuild->rebuild( st->source, camAt );
        if ( needRebuild )
        {
            st->isBusy = true;
            //const Real r = st->manualSphere->radius();
            //const Vector3 at = camAt / r;
            st->subdrive->setCameraAt( camAt );
            Request req;
            req.st = st;
            Ogre::Any any( req );
            workQueue->addRequest( workQueueChannel, 0, any );
        }
    }
}

WorkQueue::Response * IcosphereAsynch::handleRequest( const WorkQueue::Request * req, const WorkQueue * srcQ )
{
    Request r = any_cast<Request>( req->getData() );
    RequestState * st = r.st;
    st->sphere->clear();
    st->sphere->subdrive( st->subdrive );
    st->sphere->applySource( st->source );

    WorkQueue::Response * resp = OGRE_NEW WorkQueue::Response(req, true, Any());
    return resp;
}

void IcosphereAsynch::handleResponse( const WorkQueue::Response * res, const WorkQueue * srcQ )
{
    if ( (!res) || (!res->succeeded()) )
        return;

    Request r = any_cast<Request>(res->getRequest()->getData());
    RequestState * st = r.st;
    st->manualSphere->update();
    st->isBusy = false;
}

void IcosphereAsynch::initWorkQueue(void)
{
    workQueue = Root::getSingleton().getWorkQueue();
    workQueueChannel = workQueue->getChannel( "Ogre/IcoHeightmap" );
    workQueue->addResponseHandler( workQueueChannel, this );
    workQueue->addRequestHandler(  workQueueChannel, this );
}






}



