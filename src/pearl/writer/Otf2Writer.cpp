/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <config.h>
#include <pearl/Otf2Writer.h>

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <map>
#include <sstream>

#include <mpi.h>

#include <otf2/OTF2_Callbacks.h>

#include <pearl/CallbackData.h>
#include <pearl/CallbackManager.h>
#include <pearl/Callpath.h>
#include <pearl/Communicator.h>
#include <pearl/Event_rep.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/Location.h>
#include <pearl/LocationGroup.h>
#include <pearl/Metric.h>
#include <pearl/MpiCollEnd_rep.h>
#include <pearl/MpiComm.h>
#include <pearl/ProcessGroup.h>
#include <pearl/Region.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>
#include <pearl/pearl_replay.h>

#include <pearl/ThreadAcquireLock_rep.h>
#include <pearl/ThreadFork_rep.h>
#include <pearl/ThreadBegin_rep.h>
#include <pearl/ThreadEnd_rep.h>
#include <pearl/ThreadWait_rep.h>
#include <pearl/ThreadCreate_rep.h>
#include <pearl/ThreadTeamBegin_rep.h>
#include <pearl/ThreadTeamEnd_rep.h>
#include <pearl/ThreadReleaseLock_rep.h>
#include <pearl/ThreadTaskCreate_rep.h>
#include <pearl/ThreadTaskComplete_rep.h>
#include <pearl/ThreadTaskSwitch_rep.h>

#include "Otf2CollCallbacks.h"
#include "Process.h"

using namespace std;
using namespace pearl;

#define DECODE_TASK_ID( task ) \
    ( ( uint32_t )( ( task ) >> 32 ) ), \
    ( ( uint32_t )( ( task ) & 0xFFFFFFFFu ) )


namespace {

extern "C" OTF2_FlushType pre_flush(void*         userData,
                                    OTF2_FileType fileType,
                                    uint64_t      locationId,
                                    void*         callerData,
                                    bool          final);

uint64_t getTimestamp(timestamp_t time);
OTF2_RegionRole getRegionRole(const Region& region);
OTF2_Paradigm getParadigm(const Region& region);
OTF2_MetricMode getMetricMode(const Metric* metric);
OTF2_CollectiveOp getMpiCollectiveType(MpiCollEnd_rep::coll_type type);
OTF2_LocationType getLocationType(Location::Type type);
OTF2_LocationGroupType getLocationGroupType(LocationGroup::Type type);

OTF2_FlushCallbacks flush_callbacks = { pre_flush, NULL };
OTF2_CollectiveCallbacks collective_callbacks = {
    NULL,                                 // .otf2_release
    pearl::detail::otf2CollCbGetSize,     // .otf2_get_size
    pearl::detail::otf2CollCbGetRank,     // .otf2_get_rank
    NULL,                                 // .otf2_create_local_comm
    NULL,                                 // .otf2_free_local_comm
    pearl::detail::otf2CollCbBarrier,     // .otf2_barrier
    pearl::detail::otf2CollCbBroadcast,   // .otf2_bcast
    pearl::detail::otf2CollCbGather,      // .otf2_gather
    pearl::detail::otf2CollCbGatherv,     // .otf2_gatherv
    pearl::detail::otf2CollCbScatter,     // .otf2_scatter
    pearl::detail::otf2CollCbScatterv     // .otf2_scatterv
};

}   /* unnamed namespace */


/*
 *---------------------------------------------------------------------------
 *
 * struct CbData
 *
 *---------------------------------------------------------------------------
 */

struct CbData : public CallbackData
{
  CbData(const GlobalDefs& defs, OTF2_EvtWriter* evtWriter)
    : m_defs(defs),
      m_evtWriter(evtWriter)
  {
  }

  const GlobalDefs& m_defs;
  OTF2_EvtWriter*   m_evtWriter;
};


/*
 *---------------------------------------------------------------------------
 *
 * class Otf2Writer
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

Otf2Writer::Otf2Writer()
    : mNextStringId(0),
      mNextSystemTreeNodeId(0),
      mGroupOffset(0),
      mThreadGroupOffset(0),
      mThreadCommOffset(0),
      mRank(0)
{
  // Create callback manager for replay
  mCbManager = new CallbackManager();

  // Register callback methods
  mCbManager->register_callback(GROUP_ENTER,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::enter));
  mCbManager->register_callback(LEAVE,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::leave));

  mCbManager->register_callback(MPI_SEND,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::mpisend));
  mCbManager->register_callback(MPI_RECV,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::mpirecv));
  mCbManager->register_callback(MPI_SEND_REQUEST,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::mpisendreq));
  mCbManager->register_callback(MPI_SEND_COMPLETE,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::mpisendcomp));
  mCbManager->register_callback(MPI_RECV_REQUEST,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::mpirecvreq));
  mCbManager->register_callback(MPI_RECV_COMPLETE,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::mpirecvcomp));
  mCbManager->register_callback(MPI_REQUEST_TESTED,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::mpireqtested));
  mCbManager->register_callback(MPI_CANCELLED,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::mpicancelled));
  mCbManager->register_callback(MPI_COLLECTIVE_BEGIN,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::mpicollbegin));
  mCbManager->register_callback(MPI_COLLECTIVE_END,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::mpicollend));

  mCbManager->register_callback(THREAD_BEGIN,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadBegin));
  mCbManager->register_callback(THREAD_END,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadEnd));
  mCbManager->register_callback(THREAD_WAIT,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadWait));
  mCbManager->register_callback(THREAD_CREATE,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadCreate));
  mCbManager->register_callback(THREAD_TEAM_BEGIN,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadTeamBegin));
  mCbManager->register_callback(THREAD_TEAM_END,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadTeamEnd));
  mCbManager->register_callback(THREAD_FORK,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadFork));
  mCbManager->register_callback(THREAD_JOIN,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadJoin));
  mCbManager->register_callback(THREAD_ACQUIRE_LOCK,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadAcquireLock));
  mCbManager->register_callback(THREAD_RELEASE_LOCK,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadReleaseLock));
  mCbManager->register_callback(THREAD_TASK_CREATE,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadTaskCreate));
  mCbManager->register_callback(THREAD_TASK_COMPLETE,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadTaskComplete));
  mCbManager->register_callback(THREAD_TASK_SWITCH,
                                PEARL_create_callback(this,
                                                      &Otf2Writer::threadTaskSwitch));
}


Otf2Writer::~Otf2Writer()
{
  // Destroy callback manager
  delete mCbManager;
}


//--- Writing experiment data -----------------------------------------------

void Otf2Writer::write(const string&     archive_name,
                       const LocalTrace& trace,
                       const GlobalDefs& defs)
{
  // Determine earliest and latest timestamps (local)
  static timestamp_t earliest;
  static timestamp_t latest;
  #pragma omp single
  {
    earliest = DBL_MAX;;
    latest   = -DBL_MAX;
  }
  #pragma omp barrier
  #pragma omp critical
  {
    if (trace.size() > 0) {
      earliest = min(earliest, trace.begin()->getTimestamp());
      latest   = max(latest, trace.end().prev()->getTimestamp());
    }
  }
  { // FCCpx requires block
    #pragma omp barrier
  }

  static OTF2_Archive* tmp_archive;
  static int           tmp_rank;
  static uint32_t      tmp_comm_offset;
  #pragma omp master
  {
    // Determine rank of calling process
    MPI_Comm_rank(MPI_COMM_WORLD, &mRank);

    // Determine earliest and latest timestamp (global)
    timestamp_t global_earliest;
    timestamp_t global_latest;
    MPI_Reduce(&earliest, &global_earliest, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&latest,   &global_latest, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // Set up OTF2 archive
    mArchive = OTF2_Archive_Open(archive_name.c_str(),
                                 "traces",
                                 OTF2_FILEMODE_WRITE,
                                 OTF2_CHUNK_SIZE_EVENTS_DEFAULT,
                                 OTF2_CHUNK_SIZE_DEFINITIONS_DEFAULT,
                                 OTF2_SUBSTRATE_POSIX,
                                 OTF2_COMPRESSION_NONE);
    OTF2_Archive_SetFlushCallbacks(mArchive, &flush_callbacks, NULL);
    OTF2_Archive_SetCollectiveCallbacks(mArchive, &collective_callbacks,
                                        NULL, NULL, NULL);
    OTF2_Archive_SetCreator(mArchive, "PEARL trace writer");

    // Write global definitions
    if (mRank == 0) {
      OTF2_GlobalDefWriter* writer = OTF2_Archive_GetGlobalDefWriter(mArchive);

      // Write clock properties
      OTF2_GlobalDefWriter_WriteClockProperties(writer,
                                                UINT64_C(10000000000),
                                                getTimestamp(global_earliest),
                                                getTimestamp(global_latest - global_earliest));

      // Write other definitions
      writeSystemTree(writer, defs);
      writeLocations(writer, defs);
      writeRegions(writer, defs);
      writeGroups(writer, defs);
      #ifdef PEARL_ENABLE_METRICS
        // Metric definitions should only be written if events provide values
        writeMetrics(writer, defs);
      #endif
      writeCallpaths(writer, defs);
      writeCommunicators(writer, defs);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&mThreadCommOffset, 1, SCALASCA_MPI_UINT32_T, 0, MPI_COMM_WORLD);
  }
  #pragma omp barrier
  #pragma omp master
  {
    tmp_archive     = mArchive;
    tmp_rank        = mRank;
    tmp_comm_offset = mThreadCommOffset;
  }
  #pragma omp barrier
  mArchive          = tmp_archive;
  mRank             = tmp_rank;
  mThreadCommOffset = tmp_comm_offset;

  // Create local event writer
  OTF2_EvtWriter* evt_writer;
  #pragma omp critical
  {
    Location::IdType locId = trace.get_location().getId();

    evt_writer = OTF2_Archive_GetEvtWriter(mArchive, locId);
  }

  // Replay trace to write event data
  CbData data(defs, evt_writer);
  PEARL_forward_replay(trace, *mCbManager, &data);

  #pragma omp barrier
  #pragma omp master
  {
    // Close OTF2 archive
    OTF2_Archive_Close(mArchive);
  }
}


//--- Replay callback routines (private) ------------------------------------

void Otf2Writer::enter(const CallbackManager& cbManager,
                       int                    userEvent,
                       const Event&           event,
                       CallbackData*          cbData)
{
  CbData*  data   = static_cast<CbData*>(cbData);
  Region*  region = const_cast<Region*>(&event.get_cnode()->getRegion());
  uint64_t time   = getTimestamp(event->getTimestamp());

  // Handle buffer flush regions
  if (*region == data->m_defs.getFlushingRegion()) {
    OTF2_EvtWriter_BufferFlush(data->m_evtWriter,
                               NULL,
                               time,
                               getTimestamp(event.leaveptr()->getTimestamp()));
    return;
  }

  // Handle regions where measurement was turned off
  if (*region == data->m_defs.getPausingRegion()) {
    OTF2_EvtWriter_MeasurementOnOff(data->m_evtWriter,
                                    NULL,
                                    time,
                                    OTF2_MEASUREMENT_OFF);
    return;
  }

  #ifdef PEARL_ENABLE_METRICS
    writeMetricValues(data->m_evtWriter, data->m_defs, event);
  #endif

  OTF2_EvtWriter_Enter(data->m_evtWriter,
                       NULL,
                       time,
                       region->getId());
}


void Otf2Writer::leave(const CallbackManager& cbManager,
                       int                    userEvent,
                       const Event&           event,
                       CallbackData*          cbData)
{
  CbData*  data   = static_cast<CbData*>(cbData);
  Region*  region = const_cast<Region*>(&event.get_cnode()->getRegion());
  uint64_t time   = getTimestamp(event->getTimestamp());

  // Leaving buffer flush regions was already handled in enter() callback
  if (*region == data->m_defs.getFlushingRegion())
    return;

  // Handle regions where measurement was turned off
  if (*region == data->m_defs.getPausingRegion()) {
    OTF2_EvtWriter_MeasurementOnOff(data->m_evtWriter,
                                    NULL,
                                    time,
                                    OTF2_MEASUREMENT_ON);
    return;
  }

  #ifdef PEARL_ENABLE_METRICS
    writeMetricValues(data->m_evtWriter, data->m_defs, event);
  #endif

  OTF2_EvtWriter_Leave(data->m_evtWriter,
                       NULL,
                       time,
                       region->getId());
}


/// @todo Remove sending of size message once available in the receive event
void Otf2Writer::mpisend(const CallbackManager& cbManager,
                         int                    userEvent,
                         const Event&           event,
                         CallbackData*          cbData)
{
  CbData*  data = static_cast<CbData*>(cbData);
  uint64_t time = getTimestamp(event->getTimestamp());
  uint64_t size = event->getBytesSent();

  OTF2_EvtWriter_MpiSend(data->m_evtWriter,
                         NULL,
                         time,
                         event->getDestination(),
                         event->getComm()->getId(),
                         event->getTag(),
                         size);

  // Since the message size is not available on the receiver's side in EPILOG,
  // we send it using a non-blocking message ("fire and forget")
  MPI_Request request;
  MPI_Isend(&size, 1, SCALASCA_MPI_UINT64_T, event->getDestination(),
            event->getTag(), event->getComm()->getComm(), &request);
  MPI_Request_free(&request);
}


/// @todo Use getBytesReceived() call to determine message length once
///       available in the receive event
void Otf2Writer::mpirecv(const CallbackManager& cbManager,
                         int                    userEvent,
                         const Event&           event,
                         CallbackData*          cbData)
{
  CbData*  data = static_cast<CbData*>(cbData);
  uint64_t time = getTimestamp(event->getTimestamp());

  // Receive the message size from the sender
  uint64_t   size;
  MPI_Status status;
  MPI_Recv(&size, 1, SCALASCA_MPI_UINT64_T, event->getSource(), event->getTag(),
           event->getComm()->getComm(), &status);

  OTF2_EvtWriter_MpiRecv(data->m_evtWriter,
                         NULL,
                         time,
                         event->getSource(),
                         event->getComm()->getId(),
                         event->getTag(),
                         size);
}


/// @todo Remove sending of size message once available in the receive event
void Otf2Writer::mpisendreq(const CallbackManager& cbManager,
                            int                    userEvent,
                            const Event&           event,
                            CallbackData*          cbData)
{
  CbData*  data = static_cast<CbData*>(cbData);
  uint64_t time = getTimestamp(event->getTimestamp());
  uint64_t size = event->getBytesSent();

  OTF2_EvtWriter_MpiIsend(data->m_evtWriter,
                          NULL,
                          time,
                          event->getDestination(),
                          event->getComm()->getId(),
                          event->getTag(),
                          size,
                          event->getRequestId());

  // Since the message size is not available on the receiver's side in EPILOG,
  // we send it using a non-blocking message ("fire and forget")
  MPI_Request request;
  MPI_Isend(&size, 1, SCALASCA_MPI_UINT64_T, event->getDestination(),
            event->getTag(), event->getComm()->getComm(), &request);
  MPI_Request_free(&request);
}


void Otf2Writer::mpisendcomp(const CallbackManager& cbManager,
                             int                    userEvent,
                             const Event&           event,
                             CallbackData*          cbData)
{
  CbData*  data = static_cast<CbData*>(cbData);
  uint64_t time = getTimestamp(event->getTimestamp());

  OTF2_EvtWriter_MpiIsendComplete(data->m_evtWriter,
                                  NULL,
                                  time,
                                  event->getRequestId());
}


void Otf2Writer::mpirecvreq(const CallbackManager& cbManager,
                            int                    userEvent,
                            const Event&           event,
                            CallbackData*          cbData)
{
  CbData*  data = static_cast<CbData*>(cbData);
  uint64_t time = getTimestamp(event->getTimestamp());

  OTF2_EvtWriter_MpiIrecvRequest(data->m_evtWriter,
                                 NULL,
                                 time,
                                 event->getRequestId());
}


/// @todo Use getBytesReceived() call to determine message length once
///       available in the receive event
void Otf2Writer::mpirecvcomp(const CallbackManager& cbManager,
                             int                    userEvent,
                             const Event&           event,
                             CallbackData*          cbData)
{
  CbData*  data = static_cast<CbData*>(cbData);
  uint64_t time = getTimestamp(event->getTimestamp());

  // Receive the message size from the sender
  uint64_t   size;
  MPI_Status status;
  MPI_Recv(&size, 1, SCALASCA_MPI_UINT64_T, event->getSource(), event->getTag(),
           event->getComm()->getComm(), &status);

   OTF2_EvtWriter_MpiIrecv(data->m_evtWriter,
                           NULL,
                           time,
                           event->getSource(),
                           event->getComm()->getId(),
                           event->getTag(),
                           size,
                           event->getRequestId());
}


void Otf2Writer::mpireqtested(const CallbackManager& cbManager,
                              int                    userEvent,
                              const Event&           event,
                              CallbackData*          cbData)
{
  CbData*  data = static_cast<CbData*>(cbData);
  uint64_t time = getTimestamp(event->getTimestamp());

  OTF2_EvtWriter_MpiRequestTest(data->m_evtWriter,
                                NULL,
                                time,
                                event->getRequestId());
}


void Otf2Writer::mpicancelled(const CallbackManager& cbManager,
                              int                    userEvent,
                              const Event&           event,
                              CallbackData*          cbData)
{
  CbData*  data = static_cast<CbData*>(cbData);
  uint64_t time = getTimestamp(event->getTimestamp());

  OTF2_EvtWriter_MpiRequestCancelled(data->m_evtWriter,
                                     NULL,
                                     time,
                                     event->getRequestId());
}


void Otf2Writer::mpicollbegin(const CallbackManager& cbManager,
                              int                    userEvent,
                              const Event&           event,
                              CallbackData*          cbData)
{
  CbData*  data = static_cast<CbData*>(cbData);
  uint64_t time = getTimestamp(event->getTimestamp());

  OTF2_EvtWriter_MpiCollectiveBegin(data->m_evtWriter,
                                    NULL,
                                    time);
}


void Otf2Writer::mpicollend(const CallbackManager& cbManager,
                            int                    userEvent,
                            const Event&           event,
                            CallbackData*          cbData)
{
  CbData*         data     = static_cast<CbData*>(cbData);
  uint64_t        time     = getTimestamp(event->getTimestamp());
  MpiCollEnd_rep& eventRep = event_cast<MpiCollEnd_rep>(*event);

  OTF2_EvtWriter_MpiCollectiveEnd(data->m_evtWriter,
                                  NULL,
                                  time,
                                  getMpiCollectiveType(eventRep.getCollType()),
                                  event->getComm()->getId(),
                                  event->getRoot() == PEARL_NO_ID
                                    ? OTF2_UNDEFINED_UINT32
                                    : event->getRoot(),
                                  event->getBytesSent(),
                                  event->getBytesReceived());
}


void Otf2Writer::threadBegin(const CallbackManager& cbManager,
                             int                    userEvent,
                             const Event&           event,
                             CallbackData*          cbData)
{
  CbData*          data     = static_cast<CbData*>(cbData);
  uint64_t         time     = getTimestamp(event->getTimestamp());
  ThreadBegin_rep& eventRep = event_cast<ThreadBegin_rep>(*event);

  OTF2_EvtWriter_ThreadBegin(data->m_evtWriter,
                             NULL,
                             time,
                             eventRep.getThreadContingent(),
                             eventRep.getSequenceCount());
}


void Otf2Writer::threadEnd(const CallbackManager& cbManager,
                           int                    userEvent,
                           const Event&           event,
                           CallbackData*          cbData)
{
  CbData*        data     = static_cast<CbData*>(cbData);
  uint64_t       time     = getTimestamp(event->getTimestamp());
  ThreadEnd_rep& eventRep = event_cast<ThreadEnd_rep>(*event);

  OTF2_EvtWriter_ThreadEnd(data->m_evtWriter,
                           NULL,
                           time,
                           eventRep.getThreadContingent(),
                           eventRep.getSequenceCount());
}


void Otf2Writer::threadWait(const CallbackManager& cbManager,
                            int                    userEvent,
                            const Event&           event,
                            CallbackData*          cbData)
{
  CbData*         data     = static_cast<CbData*>(cbData);
  uint64_t        time     = getTimestamp(event->getTimestamp());
  ThreadWait_rep& eventRep = event_cast<ThreadWait_rep>(*event);

  OTF2_EvtWriter_ThreadWait(data->m_evtWriter,
                            NULL,
                            time,
                            eventRep.getThreadContingent(),
                            eventRep.getSequenceCount());
}


void Otf2Writer::threadCreate(const CallbackManager& cbManager,
                              int                    userEvent,
                              const Event&           event,
                              CallbackData*          cbData)
{
  CbData*           data     = static_cast<CbData*>(cbData);
  uint64_t          time     = getTimestamp(event->getTimestamp());
  ThreadCreate_rep& eventRep = event_cast<ThreadCreate_rep>(*event);

  OTF2_EvtWriter_ThreadCreate(data->m_evtWriter,
                            NULL,
                            time,
                            eventRep.getThreadContingent(),
                            eventRep.getSequenceCount());
}


void Otf2Writer::threadTeamBegin(const CallbackManager& cbManager,
                                 int                    userEvent,
                                 const Event&           event,
                                 CallbackData*          cbData)
{
  CbData*              data     = static_cast<CbData*>(cbData);
  uint64_t             time     = getTimestamp(event->getTimestamp());
  ThreadTeamBegin_rep& eventRep = event_cast<ThreadTeamBegin_rep>(*event);

  OTF2_EvtWriter_ThreadTeamBegin(data->m_evtWriter,
                                 NULL,
                                 time,
                                 mThreadCommOffset + mRank);
}


void Otf2Writer::threadTeamEnd(const CallbackManager& cbManager,
                                 int                    userEvent,
                                 const Event&           event,
                                 CallbackData*          cbData)
{
  CbData*            data     = static_cast<CbData*>(cbData);
  uint64_t           time     = getTimestamp(event->getTimestamp());
  ThreadTeamEnd_rep& eventRep = event_cast<ThreadTeamEnd_rep>(*event);

  OTF2_EvtWriter_ThreadTeamBegin(data->m_evtWriter,
                                 NULL,
                                 time,
                                 mThreadCommOffset + mRank);
}

/// @todo Fix number of locations, does not consider non-CPU locations yet
void Otf2Writer::threadFork(const CallbackManager& cbManager,
                            int                    userEvent,
                            const Event&           event,
                            CallbackData*          cbData)
{
  CbData*         data     = static_cast<CbData*>(cbData);
  uint64_t        time     = getTimestamp(event->getTimestamp());
  ThreadFork_rep& eventRep = event_cast<ThreadFork_rep>(*event);

  OTF2_EvtWriter_ThreadFork(data->m_evtWriter,
                            NULL,
                            time,
                            OTF2_PARADIGM_OPENMP,
                            eventRep.getTeamSize());
}


void Otf2Writer::threadJoin(const CallbackManager& cbManager,
                            int                    userEvent,
                            const Event&           event,
                            CallbackData*          cbData)
{
  CbData*  data = static_cast<CbData*>(cbData);
  uint64_t time = getTimestamp(event->getTimestamp());

  OTF2_EvtWriter_ThreadJoin(data->m_evtWriter,
                            NULL,
                            time,
                            OTF2_PARADIGM_OPENMP);
}


void Otf2Writer::threadAcquireLock(const CallbackManager& cbManager,
                                   int                    userEvent,
                                   const Event&           event,
                                   CallbackData*          cbData)
{
  CbData*                data     = static_cast<CbData*>(cbData);
  uint64_t               time     = getTimestamp(event->getTimestamp());
  ThreadAcquireLock_rep& eventRep = event_cast<ThreadAcquireLock_rep>(*event);

  OTF2_EvtWriter_ThreadAcquireLock(data->m_evtWriter,
                                   NULL,
                                   time,
                                   OTF2_PARADIGM_OPENMP,
                                   eventRep.getLockId(),
                                   eventRep.getAcquisitionOrder());
}


void Otf2Writer::threadReleaseLock(const CallbackManager& cbManager,
                                   int                    userEvent,
                                   const Event&           event,
                                   CallbackData*          cbData)
{
  CbData*                data     = static_cast<CbData*>(cbData);
  uint64_t               time     = getTimestamp(event->getTimestamp());
  ThreadReleaseLock_rep& eventRep = event_cast<ThreadReleaseLock_rep>(*event);

  OTF2_EvtWriter_ThreadReleaseLock(data->m_evtWriter,
                                   NULL,
                                   time,
                                   OTF2_PARADIGM_OPENMP,
                                   eventRep.getLockId(),
                                   eventRep.getAcquisitionOrder());
}


void Otf2Writer::threadTaskCreate(const CallbackManager& cbManager,
                                  int                    userEvent,
                                  const Event&           event,
                                  CallbackData*          cbData)
{
  CbData*               data     = static_cast<CbData*>(cbData);
  uint64_t              time     = getTimestamp(event->getTimestamp());
  ThreadTaskCreate_rep& eventRep = event_cast<ThreadTaskCreate_rep>(*event);

  OTF2_EvtWriter_ThreadTaskCreate(data->m_evtWriter,
                                  NULL,
                                  time,
                                  eventRep.getThreadTeam(),
                                  DECODE_TASK_ID(eventRep.getTaskId()));
}


void Otf2Writer::threadTaskComplete(const CallbackManager& cbManager,
                                    int                    userEvent,
                                    const Event&           event,
                                    CallbackData*          cbData)
{
  CbData*                 data     = static_cast<CbData*>(cbData);
  uint64_t                time     = getTimestamp(event->getTimestamp());
  ThreadTaskComplete_rep& eventRep = event_cast<ThreadTaskComplete_rep>(*event);

  OTF2_EvtWriter_ThreadTaskComplete(data->m_evtWriter,
                                 NULL,
                                 time,
                                 eventRep.getThreadTeam(),
                                 DECODE_TASK_ID(eventRep.getTaskId()));
}


void Otf2Writer::threadTaskSwitch(const CallbackManager& cbManager,
                                  int                    userEvent,
                                  const Event&           event,
                                  CallbackData*          cbData)
{
  CbData*               data     = static_cast<CbData*>(cbData);
  uint64_t              time     = getTimestamp(event->getTimestamp());
  ThreadTaskSwitch_rep& eventRep = event_cast<ThreadTaskSwitch_rep>(*event);

  OTF2_EvtWriter_ThreadTaskSwitch(data->m_evtWriter,
                                  NULL,
                                  time,
                                  eventRep.getThreadTeam(),
                                  DECODE_TASK_ID(eventRep.getTaskId()));
}


//--- Internal I/O routines (private) ---------------------------------------

uint32_t Otf2Writer::writeString(OTF2_GlobalDefWriter* writer,
                                 const string& str)
{
  // Search for string in lookup table
  map<string, uint32_t>::iterator it = mStringMap.lower_bound(str);
  if ((it != mStringMap.end()) &&
      !(mStringMap.key_comp()(str, it->first))) {
    // Found ==> return ID
    return it->second;
  }

  // Not found => Create new entry
  OTF2_GlobalDefWriter_WriteString(writer, mNextStringId, str.c_str());
  mStringMap.insert(it, map<string,uint32_t>::value_type(str, mNextStringId));

  return mNextStringId++;
}


void Otf2Writer::writeSystemTree(OTF2_GlobalDefWriter* writer,
                                 const GlobalDefs&     defs)
{
  uint32_t numNodes = defs.numSystemNodes();
  for (uint32_t id = 0; id < numNodes; ++id) {
    const SystemNode& node = defs.getSystemNode(id);
    SystemNode* parent     = node.getParent();

    uint32_t nameId  = writeString(writer, node.getName().getString());
    uint32_t classId = writeString(writer, node.getClassName().getString());
    OTF2_GlobalDefWriter_WriteSystemTreeNode(writer,
                                             node.getId(),
                                             nameId,
                                             classId,
                                             (NULL == parent)
                                               ? OTF2_UNDEFINED_UINT32
                                               : parent->getId());
  }
}


/// @todo Add location/location group type handling
void Otf2Writer::writeLocations(OTF2_GlobalDefWriter* writer,
                                const GlobalDefs&     defs)
{
  uint32_t numGroups = defs.numLocationGroups();
  for (uint32_t id = 0; id < numGroups; ++id) {
    const LocationGroup& group = defs.getLocationGroup(id);

    uint32_t nameId = writeString(writer, group.getName().getString());
    OTF2_GlobalDefWriter_WriteLocationGroup(writer,
                                            group.getId(),
                                            nameId,
                                            getLocationGroupType(group.getType()),
                                            group.getParent()->getId());

    uint32_t numLocations = group.numLocations();
    for (uint32_t index = 0; index < numLocations; ++index) {
      const Location& location = group.getLocation(index);

      uint32_t lNameId = writeString(writer, location.getName().getString());
      OTF2_GlobalDefWriter_WriteLocation(writer,
                                         location.getId(),
                                         lNameId,
                                         getLocationType(location.getType()),
                                         OTF2_UNDEFINED_UINT64,
                                         location.getParent()->getId());
    }
  }
}


/// @todo Handle region flags
void Otf2Writer::writeRegions(OTF2_GlobalDefWriter* writer,
                              const GlobalDefs&     defs)
{
  // Write region definitions
  for (uint32_t index = 0; index < defs.numRegions(); index++) {
    const Region& region = defs.getRegion(index);

    uint32_t canonicalNameId = writeString(writer, region.getCanonicalName().getString());
    uint32_t displayNameId   = writeString(writer, region.getDisplayName().getString());
    uint32_t descId          = writeString(writer, region.getDescription().getString());
    uint32_t fileId          = writeString(writer, region.getFilename().getString());

    line_t line = region.getStartLine();
    uint32_t beginLine = (line == PEARL_NO_NUM) ? 0 : line;

    line = region.getEndLine();
    uint32_t endLine = (line == PEARL_NO_NUM) ? 0 : line;

    OTF2_GlobalDefWriter_WriteRegion(writer,
                                     region.getId(),
                                     displayNameId,
                                     canonicalNameId,
                                     descId,
                                     getRegionRole(region),
                                     getParadigm(region),
                                     OTF2_REGION_FLAG_NONE,
                                     fileId,
                                     beginLine,
                                     endLine);
  }
}


/// @todo Handle location, region and metric groups
void Otf2Writer::writeGroups(OTF2_GlobalDefWriter* writer,
                             const GlobalDefs&     defs)
{
  uint32_t emptyNameId = writeString(writer, "");

  // Write MPI locations group definition
  uint32_t  count   = defs.numLocationGroups();
  uint64_t* members = new uint64_t[count];
  for (uint32_t index = 0; index < count; ++index) {
    const LocationGroup& group = defs.getLocationGroup(index);
    if (LocationGroup::TYPE_PROCESS != group.getType())
      continue;

    const Process& process      = static_cast<const Process&>(group);
    uint32_t       numLocations = process.numLocations();
    for (uint32_t locIndex = 0; locIndex < numLocations; ++locIndex) {
      const Location& location = process.getLocation(locIndex);
      if (0 == location.getThreadId())
        members[process.getRank()] = location.getId();
    }
  }
  OTF2_GlobalDefWriter_WriteGroup(writer,
                                  0,
                                  emptyNameId,
                                  OTF2_GROUP_TYPE_COMM_LOCATIONS,
                                  OTF2_PARADIGM_MPI,
                                  OTF2_GROUP_FLAG_NONE,
                                  count,
                                  members);
  delete[] members;

  // Write MPI group definitions
  for (uint32_t index = 0; index < defs.numProcessGroups(); index++) {
    ProcessGroup*        group       = defs.get_group(index);
    ProcessGroup::IdType groupId     = group->getId();
    uint32_t             groupNameId = writeString(writer, group->getName().getString());

    // Check whether groups have to be renumbered:
    // Groups are stored in ID order, i.e., if needed, the first group will
    // already trigger this conditional and all subsequent group IDs are
    // shifted by one
    if (groupId == 0)
        mGroupOffset = 1;

    // Set up ranks array
    count   = group->numRanks();
    members = new uint64_t[count];
    for (uint32_t rank = 0; rank < count; rank++)
      members[rank] = group->getGlobalRank(rank);

    OTF2_GlobalDefWriter_WriteGroup(writer,
                                    groupId + mGroupOffset,
                                    groupNameId,
                                    group->isSelf()
                                      ? OTF2_GROUP_TYPE_COMM_SELF
                                      : OTF2_GROUP_TYPE_COMM_GROUP,
                                    OTF2_PARADIGM_MPI,
                                    OTF2_GROUP_FLAG_NONE,
                                    group->isSelf()
                                      ? 0
                                      : count,
                                    members);

    // Free resources
    delete[] members;

    // Store ID of first thread team group
    mThreadGroupOffset = groupId + mGroupOffset + 1;
  }

#ifdef _OPENMP
  // Write OpenMP locations group definition
  uint32_t nextId = mThreadGroupOffset++;
  map<Location::IdType, uint32_t> idMap;
  count   = defs.numLocations();
  members = new uint64_t[count];
  for (uint32_t index = 0; index < count; ++index) {
    const Location&  location = defs.getLocationByIndex(index);
    Location::IdType locId    = location.getId();

    // Store group members and mapping  
    members[index] = locId;
    idMap[locId]   = index;
  }
  uint32_t groupNameId = writeString(writer, "OpenMP");
  OTF2_GlobalDefWriter_WriteGroup(writer,
                                  nextId++,
                                  groupNameId,
                                  OTF2_GROUP_TYPE_COMM_LOCATIONS,
                                  OTF2_PARADIGM_OPENMP,
                                  OTF2_GROUP_FLAG_NONE,
                                  count,
                                  members);
  delete[] members;

  // Write thread team groups
  count = defs.numLocationGroups();
  for (uint32_t index = 0; index < count; ++index) {
    const LocationGroup& group = defs.getLocationGroup(index);
    if (LocationGroup::TYPE_PROCESS != group.getType())
      continue;

    const Process& process      = static_cast<const Process&>(group);
    uint32_t       numLocations = process.numLocations();
    members = new uint64_t[numLocations];
    for (uint32_t locIndex = 0; locIndex < numLocations; ++locIndex) {
      const Location&  location = process.getLocation(locIndex);
      Location::IdType locId    = location.getId();

      members[locIndex] = idMap[locId];
    }

    OTF2_GlobalDefWriter_WriteGroup(writer,
                                    nextId++,
                                    emptyNameId,
                                    OTF2_GROUP_TYPE_COMM_GROUP,
                                    OTF2_PARADIGM_OPENMP,
                                    OTF2_GROUP_FLAG_NONE,
                                    numLocations,
                                    members);
    delete[] members;
  }
#endif   // !_OPENMP
}


void Otf2Writer::writeCallpaths(OTF2_GlobalDefWriter* writer,
                                const GlobalDefs&     defs)
{
  // Write callpath definitions
  for (uint32_t index = 0; index < defs.numCallpaths(); index++) {
    Callpath* callpath = defs.get_cnode(index);
    Callpath* parent   = callpath->getParent();

    OTF2_GlobalDefWriter_WriteCallpath(writer,
                                       callpath->getId(),
                                       parent
                                         ? parent->getId()
                                         : OTF2_UNDEFINED_UINT32,
                                       callpath->getRegion().getId());
  }
}


void Otf2Writer::writeMetrics(OTF2_GlobalDefWriter* writer,
                              const GlobalDefs&     defs)
{
  // Set up list of metric IDs
  uint32_t  count   = defs.num_metrics();
  uint32_t* metrics = new uint32_t[count];

  // Write hardware counter metric definitions
  for (uint32_t index = 0; index < count; index++) {
    Metric* metric = defs.get_metric(index);

    metrics[index] = index;

    uint32_t nameId = writeString(writer, metric->get_name());
    uint32_t descId = writeString(writer, metric->get_description());
    uint32_t unitId = writeString(writer, "#");

    OTF2_GlobalDefWriter_WriteMetricMember(writer,
                                           metric->get_id(),
                                           nameId,
                                           descId,
                                           OTF2_METRIC_TYPE_PAPI,
                                           getMetricMode(metric),
                                           (metric->get_type() == "INTEGER")
                                             ? OTF2_TYPE_UINT64
                                             : OTF2_TYPE_DOUBLE,
                                           OTF2_BASE_DECIMAL,
                                           0,
                                           unitId);
  }

  // Write single metric class, as metrics are always stored "en bloc"
  OTF2_GlobalDefWriter_WriteMetricClass(writer,
                                        0,
                                        count,
                                        metrics,
                                        OTF2_METRIC_SYNCHRONOUS_STRICT,
                                        OTF2_RECORDER_KIND_CPU);

  // Free resources
  delete[] metrics;
}


void Otf2Writer::writeCommunicators(OTF2_GlobalDefWriter* writer,
                                    const GlobalDefs&     defs)
{
  // Write MPI communicator definitions
  for (uint32_t index = 0; index < defs.numCommunicators(); index++) {
    const Communicator& comm = defs.getCommunicatorByIndex(index);
    uint32_t      commNameId = writeString(writer, comm.getName().getString());

    OTF2_GlobalDefWriter_WriteComm(writer,
                                   comm.getId(),
                                   commNameId,
                                   comm.getGroup().getId() + mGroupOffset,
                                   (comm.getParent() == NULL)
                                     ? OTF2_UNDEFINED_UINT32
                                     : comm.getParent()->getId());

    // Store ID of first thread team communicator
    mThreadCommOffset = comm.getId() + 1;
  }

#ifdef _OPENMP
  // Write OpenMP thread team communicators
  uint32_t nextId      = mThreadCommOffset;
  uint32_t emptyNameId = writeString(writer, "");
  uint32_t count       = defs.numLocationGroups();
  for (uint32_t index = 0; index < count; ++index) {
    const LocationGroup& group = defs.getLocationGroup(index);
    if (LocationGroup::TYPE_PROCESS != group.getType())
      continue;

    const Process& process = static_cast<const Process&>(group);
    OTF2_GlobalDefWriter_WriteComm(writer,
                                   nextId++,
                                   emptyNameId,
                                   mThreadGroupOffset + process.getRank(),
                                   OTF2_UNDEFINED_UINT32);
  }
#endif   // _OPENMP
}


void Otf2Writer::writeMetricValues(OTF2_EvtWriter*   writer,
                                   const GlobalDefs& defs,
                                   const Event&      event)
{
  #ifdef PEARL_ENABLE_METRICS
    uint32_t count = defs.num_metrics();
    if (0 == count)
        return;

    // Create temporary storage
    OTF2_Type*        types  = new OTF2_Type[count];
    OTF2_MetricValue* values = new OTF2_MetricValue[count];

    // Set up temporaries
    for (uint32_t index = 0; index < count; index++) {
      Metric* metric = defs.get_metric(index);

      types[index] = (metric->get_type() == "INTEGER")
                       ? OTF2_TYPE_UINT64
                       : OTF2_TYPE_DOUBLE;
      values[index].floating_point = event->getMetric(index);
    }

    OTF2_EvtWriter_Metric(writer,
                          NULL,
                          getTimestamp(event->getTimestamp()),
                          0,
                          count,
                          types,
                          values);

    // Release temporary storage
    delete[] values;
    delete[] types;
  #endif
}


//--- Helper functions ------------------------------------------------------

namespace
{


OTF2_FlushType pre_flush(void*         userData,
                         OTF2_FileType fileType,
                         uint64_t      locationId,
                         void*         callerData,
                         bool          final)
{
  return OTF2_FLUSH;
}


uint64_t getTimestamp(timestamp_t time)
{
  return static_cast<uint64_t>(time * 1.0e+10);
}


OTF2_RegionRole getRegionRole(const Region& region)
{
  switch (region.getRole()) {
    case Region::ROLE_UNKNOWN:
      return OTF2_REGION_ROLE_UNKNOWN;

    case Region::ROLE_FUNCTION:
      return OTF2_REGION_ROLE_FUNCTION;

    case Region::ROLE_WRAPPER:
      return OTF2_REGION_ROLE_WRAPPER;

    case Region::ROLE_LOOP:
      return OTF2_REGION_ROLE_LOOP;

    case Region::ROLE_CODE:
      return OTF2_REGION_ROLE_CODE;

    case Region::ROLE_PARALLEL:
      return OTF2_REGION_ROLE_PARALLEL;

    case Region::ROLE_SECTIONS:
      return OTF2_REGION_ROLE_SECTIONS;

    case Region::ROLE_SECTION:
      return OTF2_REGION_ROLE_SECTION;

    case Region::ROLE_SINGLE:
      return OTF2_REGION_ROLE_SINGLE;

    case Region::ROLE_SINGLE_SBLOCK:
      return OTF2_REGION_ROLE_SINGLE_SBLOCK;

    case Region::ROLE_WORKSHARE:
      return OTF2_REGION_ROLE_WORKSHARE;

    case Region::ROLE_MASTER:
      return OTF2_REGION_ROLE_MASTER;

    case Region::ROLE_CRITICAL:
      return OTF2_REGION_ROLE_CRITICAL;

    case Region::ROLE_CRITICAL_SBLOCK:
      return OTF2_REGION_ROLE_CRITICAL_SBLOCK;

    case Region::ROLE_BARRIER:
      return OTF2_REGION_ROLE_BARRIER;

    case Region::ROLE_IMPLICIT_BARRIER:
      return OTF2_REGION_ROLE_IMPLICIT_BARRIER;

    case Region::ROLE_ATOMIC:
      return OTF2_REGION_ROLE_ATOMIC;

    case Region::ROLE_FLUSH:
      return OTF2_REGION_ROLE_FLUSH;

    case Region::ROLE_ORDERED:
      return OTF2_REGION_ROLE_ORDERED;

    case Region::ROLE_ORDERED_SBLOCK:
      return OTF2_REGION_ROLE_ORDERED_SBLOCK;

    case Region::ROLE_TASK:
      return OTF2_REGION_ROLE_TASK;

    case Region::ROLE_TASK_CREATE:
      return OTF2_REGION_ROLE_TASK_CREATE;

    case Region::ROLE_TASK_WAIT:
      return OTF2_REGION_ROLE_TASK_WAIT;

    case Region::ROLE_COLL_ONE2ALL:
      return OTF2_REGION_ROLE_COLL_ONE2ALL;

    case Region::ROLE_COLL_ALL2ONE:
      return OTF2_REGION_ROLE_COLL_ALL2ONE;

    case Region::ROLE_COLL_ALL2ALL:
      return OTF2_REGION_ROLE_COLL_ALL2ALL;

    case Region::ROLE_COLL_OTHER:
      return OTF2_REGION_ROLE_COLL_OTHER;

    case Region::ROLE_FILE_IO:
      return OTF2_REGION_ROLE_FILE_IO;

    case Region::ROLE_POINT2POINT:
      return OTF2_REGION_ROLE_POINT2POINT;

    case Region::ROLE_RMA:
      return OTF2_REGION_ROLE_RMA;

    case Region::ROLE_DATA_TRANSFER:
      return OTF2_REGION_ROLE_DATA_TRANSFER;

    case Region::ROLE_ARTIFICIAL:
      return OTF2_REGION_ROLE_ARTIFICIAL;

    case Region::ROLE_THREAD_CREATE:
      return OTF2_REGION_ROLE_THREAD_CREATE;

    case Region::ROLE_THREAD_WAIT:
      return OTF2_REGION_ROLE_THREAD_WAIT;

    case Region::ROLE_TASK_UNTIED:
      return OTF2_REGION_ROLE_TASK_UNTIED;

    case Region::ROLE_ALLOCATE:
      return OTF2_REGION_ROLE_ALLOCATE;

    case Region::ROLE_DEALLOCATE:
      return OTF2_REGION_ROLE_DEALLOCATE;

    case Region::ROLE_REALLOCATE:
      return OTF2_REGION_ROLE_REALLOCATE;

    default:
      break;
  }

  // Everything else
  return OTF2_REGION_ROLE_UNKNOWN;
}


OTF2_Paradigm getParadigm(const Region& region)
{
  switch (native_value(region.getParadigm())) {
    case Paradigm::UNKNOWN:
      return OTF2_PARADIGM_UNKNOWN;

    case Paradigm::USER:
      return OTF2_PARADIGM_USER;

    case Paradigm::COMPILER:
      return OTF2_PARADIGM_COMPILER;

    case Paradigm::OPENMP:
      return OTF2_PARADIGM_OPENMP;

    case Paradigm::MPI:
      return OTF2_PARADIGM_MPI;

    case Paradigm::CUDA:
      return OTF2_PARADIGM_CUDA;

    case Paradigm::MEASUREMENT_SYSTEM:
      return OTF2_PARADIGM_MEASUREMENT_SYSTEM;

    case Paradigm::PTHREAD:
      return OTF2_PARADIGM_PTHREAD;

    case Paradigm::HMPP:
      return OTF2_PARADIGM_HMPP;

    case Paradigm::OMPSS:
      return OTF2_PARADIGM_OMPSS;

    case Paradigm::HARDWARE:
      return OTF2_PARADIGM_HARDWARE;

    case Paradigm::GASPI:
      return OTF2_PARADIGM_GASPI;

    case Paradigm::UPC:
      return OTF2_PARADIGM_UPC;

    case Paradigm::SHMEM:
      return OTF2_PARADIGM_SHMEM;

    case Paradigm::WINTHREAD:
        return OTF2_PARADIGM_WINTHREAD;

    case Paradigm::QTTHREAD:
        return OTF2_PARADIGM_QTTHREAD;

    case Paradigm::ACETHREAD:
        return OTF2_PARADIGM_ACETHREAD;

    case Paradigm::TBBTHREAD:
        return OTF2_PARADIGM_TBBTHREAD;

    case Paradigm::OPENACC:
        return OTF2_PARADIGM_OPENACC;

    case Paradigm::OPENCL:
        return OTF2_PARADIGM_OPENCL;

    case Paradigm::MTAPI:
        return OTF2_PARADIGM_MTAPI;

    case Paradigm::SAMPLING:
        return OTF2_PARADIGM_SAMPLING;

    case Paradigm::NONE:
        return OTF2_PARADIGM_NONE;

    // For "NUMBER_OF_PARADIGMS" -- to make the compiler happy...
    default:
       break;
  }

  // Since all possible paradigms should be handled in the switch
  // statement above, something went wrong if we reach this point...
  assert(false);
  return OTF2_PARADIGM_UNKNOWN;
}


OTF2_MetricMode getMetricMode(const Metric* metric)
{
  if (metric->get_mode() == "COUNTER") {
    if (metric->get_interval() == "START")
      return OTF2_METRIC_ACCUMULATED_START;

    if (metric->get_interval() == "LAST")
      return OTF2_METRIC_ACCUMULATED_LAST;

    if (metric->get_interval() == "NEXT")
      return OTF2_METRIC_ACCUMULATED_NEXT;
  }

  // Never seen SAMPLE/RATE metrics in real EPILOG traces -- store as sample
  // just in case...
  return OTF2_METRIC_ABSOLUTE_POINT;
}


OTF2_CollectiveOp getMpiCollectiveType(MpiCollEnd_rep::coll_type type)
{
  switch (type) {
    case MpiCollEnd_rep::BARRIER:
      return OTF2_COLLECTIVE_OP_BARRIER;

    case MpiCollEnd_rep::BCAST:
      return OTF2_COLLECTIVE_OP_BCAST;

    case MpiCollEnd_rep::GATHER:
      return OTF2_COLLECTIVE_OP_GATHER;

    case MpiCollEnd_rep::GATHERV:
      return OTF2_COLLECTIVE_OP_GATHERV;

    case MpiCollEnd_rep::SCATTER:
      return OTF2_COLLECTIVE_OP_SCATTER;

    case MpiCollEnd_rep::SCATTERV:
      return OTF2_COLLECTIVE_OP_SCATTERV;

    case MpiCollEnd_rep::ALLGATHER:
      return OTF2_COLLECTIVE_OP_ALLGATHER;

    case MpiCollEnd_rep::ALLGATHERV:
      return OTF2_COLLECTIVE_OP_ALLGATHERV;

    case MpiCollEnd_rep::ALLTOALL:
      return OTF2_COLLECTIVE_OP_ALLTOALL;

    case MpiCollEnd_rep::ALLTOALLV:
      return OTF2_COLLECTIVE_OP_ALLTOALLV;

    case MpiCollEnd_rep::ALLTOALLW:
      return OTF2_COLLECTIVE_OP_ALLTOALLW;

    case MpiCollEnd_rep::ALLREDUCE:
      return OTF2_COLLECTIVE_OP_ALLREDUCE;

    case MpiCollEnd_rep::REDUCE:
      return OTF2_COLLECTIVE_OP_REDUCE;

    case MpiCollEnd_rep::REDUCE_SCATTER:
      return OTF2_COLLECTIVE_OP_REDUCE_SCATTER;

    case MpiCollEnd_rep::SCAN:
      return OTF2_COLLECTIVE_OP_SCAN;

    case MpiCollEnd_rep::EXSCAN:
      return OTF2_COLLECTIVE_OP_EXSCAN;

    case MpiCollEnd_rep::REDUCE_SCATTER_BLOCK:
      return OTF2_COLLECTIVE_OP_REDUCE_SCATTER_BLOCK;

    default:
      break;
  }

  // This point should never be reached...
  assert(false);
  return 255;
}


OTF2_LocationType getLocationType(Location::Type type)
{
  switch (type) {
    case Location::TYPE_UNKNOWN:
      return OTF2_LOCATION_TYPE_UNKNOWN;

    case Location::TYPE_CPU_THREAD:
      return OTF2_LOCATION_TYPE_CPU_THREAD;

    case Location::TYPE_GPU:
      return OTF2_LOCATION_TYPE_GPU;

    case Location::TYPE_METRIC:
      return OTF2_LOCATION_TYPE_METRIC;

    default:
      break;
  }

  // This point should never be reached...
  assert(false);
  return OTF2_LOCATION_TYPE_UNKNOWN;
}


OTF2_LocationGroupType getLocationGroupType(LocationGroup::Type type)
{
  switch (type) {
    case LocationGroup::TYPE_UNKNOWN:
      return OTF2_LOCATION_GROUP_TYPE_UNKNOWN;

    case LocationGroup::TYPE_PROCESS:
      return OTF2_LOCATION_GROUP_TYPE_PROCESS;

    default:
      break;
  }

  // This point should never be reached...
  assert(false);
  return OTF2_LOCATION_GROUP_TYPE_UNKNOWN;
}

}   /* unnamed namespace */
