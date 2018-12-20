
#ifndef __ICOSPHERE_ASYNCH_H_
#define __ICOSPHERE_ASYNCH_H_

#include "OgreWorkQueue.h"
#include "icosphere.h"
#include "manual_sphere.h"

namespace IcoHeightmap
{

struct RequestState
{
    // Stuff to work with.
    ManualSphere * manualSphere;
    Icosphere    * sphere;
    NeedSubdrive * subdrive;
    NeedRebuild  * rebuild;
    Source       * source;
    bool         isBusy;
};

struct Request
{
    RequestState * st;

    friend std::ostream& operator<<( std::ostream & o, const Request & r )
    {
        return o;
    }
};

class IcosphereAsynch: public WorkQueue::RequestHandler, public WorkQueue::ResponseHandler
{
public:
    IcosphereAsynch();
    ~IcosphereAsynch();


    void handleAsynchResults( RequestState * st, const Vector3 & camAt );

    /// Implementation for WorkQueue::RequestHandler
    WorkQueue::Response* handleRequest(const WorkQueue::Request* req, const WorkQueue* srcQ);
    /// Implementation for WorkQueue::ResponseHandler
    void handleResponse(const WorkQueue::Response* res, const WorkQueue* srcQ);

private:
    /** Initializes the WorkQueue (once).
    */
    void initWorkQueue(void);


    /// The workqueue load request.
    static const uint16 WORKQUEUE_LOAD_REQUEST;

    /// The workqueue.
    WorkQueue * workQueue;

    /// The workqueue channel.
    uint16 workQueueChannel;



};




}




#endif




