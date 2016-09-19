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


#include <config.h>
#include "Synchronizer.h"

#include <algorithm>
#include <cfloat>
#include <cstdio>

#ifdef _OPENMP
  #include "OmpCommunication.h"
  #include "OmpData.h"
#endif   // _OPENMP

#include <elg_error.h>

#include <pearl/Location.h>
#include <pearl/MpiCollEnd_rep.h>
#include <pearl/MpiComm.h>
#include <pearl/ProcessGroup.h>
#include <pearl/Region.h>
#include <pearl/String.h>
#include <pearl/pearl_replay.h>

#include "AmortData.h"
#include "Clock.h"
#include "FwdAmortData.h"
#include "IntervalStat.h"
#include "Latency.h"
#include "Network.h"
#include "Operator.h"
#include "Predicates.h"
#include "RuntimeStat.h"
#include "TimeStat.h"

using namespace std;
using namespace pearl;


Synchronizer::Synchronizer(int r, const LocalTrace& myTrace)
  : trace(myTrace),
    firstEvtT(-DBL_MAX),
    max_error(0.01),
    m_max_slope(0.01),
    threshold(0.05),
    m_rank(r),
    num_sends(0),
    t_stat(0),
    i_stat(0),
    tsa(0)
{
  init();
}


Synchronizer::Synchronizer(int r, const LocalTrace& myTrace,
                           TimeStat* time_stat, IntervalStat* interval_stat)
  : trace(myTrace),
    firstEvtT(-DBL_MAX),
    max_error(0.01),
    m_max_slope(0.01),
    threshold(0.05),
    m_rank(r),
    num_sends(0),
    t_stat(time_stat),
    i_stat(interval_stat),
    tsa(0)
{
  init();
}


Synchronizer::~Synchronizer()
{
  delete m_clc;
  delete m_network;
  delete r_stat;

  delete[] tsa;

  delete_requests();
  m_violations.clear();
  #pragma omp master
  {
    free_operators();
  }
}


void Synchronizer::init()
{
  // Get location
  const Location& loc = trace.get_location();
  m_loc.machine = loc.getMachineId();
  m_loc.node    = loc.getNodeId();
  m_loc.process = loc.getRank();
  m_loc.thread  = loc.getThreadId();

  // Create Latency
  Latency* latency = new Latency(0.0, 1.0e-6, 1.0e-6, 1.0e-6, 1.0e-6, 0.01e-6);

  // Create Network with Latency
  m_network = new Network(m_loc, latency);

  // Create clock
  m_clc = new Clock;

  // Create runtime statistics
  r_stat = new RuntimeStat(1);

  // Initialize operator and datatypes
  #pragma omp master
  {
    init_operators();
  }
}


void Synchronizer::prepare_replay()
{
  // Next pass
  r_stat->set_pass();

  // Reset clock
  m_clc->reset();

  // Apply controller to obtain controll variable for next pass
  m_clc->apply_controller(r_stat->get_pass());

  // Clear clock violation list
  m_violations.clear();

  // Reset number of send events for subsequent replay
  num_sends = 0;

  // Copy/restore timestamps for subsequent replay
  if (r_stat->get_pass() == 1)
    get_ts();
  else
    set_ts();
}


/**
 *  Used to synchronize event stream
 **/
void Synchronizer::synchronize()
{
  forward_amortization();
  backward_amortization();
}


void Synchronizer::forward_amortization()
{
  timestamp_t gl_error = -DBL_MAX;
  elg_cntl_msg("[CLC]: Forward amortization... ");

  if(t_stat != 0 && i_stat != 0) {
    t_stat->get_org_timestamps();
    i_stat->get_org_timestamps();
  }

  // Set up callback manager
  CallbackManager* cbmanager = new CallbackManager;
  cbmanager->register_callback (GROUP_SEND, PEARL_create_callback(this, &Synchronizer::amortize_fwd_send ));
  cbmanager->register_callback (GROUP_RECV, PEARL_create_callback(this, &Synchronizer::amortize_fwd_recv ));
  cbmanager->register_callback (MPI_COLLECTIVE_END, PEARL_create_callback(this, &Synchronizer::amortize_fwd_coll ));
#ifdef _OPENMP
  cbmanager->register_callback (THREAD_FORK, PEARL_create_callback(this, &Synchronizer::amortize_fwd_fork ));
  cbmanager->register_callback (THREAD_JOIN, PEARL_create_callback(this, &Synchronizer::amortize_fwd_join ));
  cbmanager->register_callback (THREAD_TEAM_BEGIN, PEARL_create_callback(this, &Synchronizer::amortize_fwd_ttb ));
  cbmanager->register_callback (THREAD_TEAM_END, PEARL_create_callback(this, &Synchronizer::amortize_fwd_tte ));
  cbmanager->register_callback (LEAVE, PEARL_create_callback(this, &Synchronizer::amortize_fwd_leave ));
#endif   // _OPENMP

  // Perform the replay
  do {
    // Prepare Synchronizer for subsequent replay
    prepare_replay();

    // Replay the target application and synchronize timestamps...
    { // FCCpx requires block
      #pragma omp barrier
    }
    FwdAmortData data(this);
    PEARL_forward_replay(trace, *cbmanager, &data);
    #pragma omp barrier

    // Until accuracy is sufficient
    gl_error = r_stat->get_clock_error();
  } while (gl_error > threshold && r_stat->next_pass());

  // Release callback manager
  delete cbmanager;
}


void Synchronizer::backward_amortization()
{
  // If clock condition violations occured -> apply backward amortization
  timestamp_t gl_violations = 0.0;
  timestamp_t violations    = (timestamp_t) m_clc->get_num_viol();
#ifdef _OPENMP
  OMP_Allreduce_sum(violations, omp_global_sum_timestamp);
  violations = omp_global_sum_timestamp;
#endif   // _OPENMP

  #pragma omp master
  {
    MPI_Allreduce(&violations, &gl_violations, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  }

#ifdef _OPENMP
  OMP_Bcast(gl_violations, omp_global_sum_timestamp);
  gl_violations = omp_global_sum_timestamp;
#endif   // _OPENMP

  if(t_stat != 0 && i_stat != 0) {
    t_stat->get_fa_timestamps();
    i_stat->get_fa_timestamps();
  }

  if (gl_violations > 0.0) {
    elg_cntl_msg("[CLC]: Backward amortization... ");
    #pragma omp barrier
    ba_amortize();
    #pragma omp barrier
  } else {
    elg_cntl_msg("[CLC]: Skipping backward amortization... ");
  }

  if(t_stat != 0 && i_stat != 0) {
    t_stat->get_ba_timestamps();
    i_stat->get_ba_timestamps();
  }

  // Release timestamp array
  delete[] tsa;
  tsa = 0;
}


void Synchronizer::amortize_fwd_send(const CallbackManager& cbmanager,
                                     int user_event, const Event& event,
                                     CallbackData* cdata)
{
  // Apply controlled logical clock (internal amortization)
  fa_amortize_intern(event);

  // Pack & send timestamp of SEND event
  amortization_data *local = new amortization_data;
  *local = pack_data(m_loc, event->getTimestamp());

  MPI_Request request;
  MPI_Isend(local, 1, AMORT_DATA, event->getDestination(),
            event->getTag(), event->getComm()->getComm(), &request);
  m_requests.push_back(request);
  m_data.push_back(local);
  update_requests();

  // No further amortization necessary
  FwdAmortData* data = static_cast<FwdAmortData*>(cdata);
  data->setAmortized();
}


void Synchronizer::amortize_fwd_recv(const CallbackManager& cbmanager,
                                     int user_event, const Event& event,
                                     CallbackData* cdata)
{
  // Receive timestamp of SEND event and apply amortization

  // Get corresponding timestamp, location, and event type via replay
  clc_location ex_loc;
  amortization_data ex_object;

  // Receive send event time 
  MPI_Status stat;
  MPI_Recv(&ex_object, 1, AMORT_DATA, event->getSource(),
           event->getTag(), event->getComm()->getComm(), &stat);
  cur_event_type = CLC_P2P;
  timestamp_t sendEvtT = unpack_data(ex_loc, ex_object);

  fa_amortize_recv(event, sendEvtT, ex_loc);

  // No further amortization necessary
  FwdAmortData* data = static_cast<FwdAmortData*>(cdata);
  data->setAmortized();
}


void Synchronizer::amortize_fwd_coll(const CallbackManager& cbmanager,
                                     int user_event, const Event& event,
                                     CallbackData* cdata)
{
  // Set current event type
  cur_event_type = CLC_COLL;

  // Determine corresponding BEGIN event and region
  Event begin          = event.beginptr();
  const Region& region = begin.enterptr()->getRegion();

  // Pack local data
  amortization_data local_object = pack_data(m_loc, begin->getTimestamp());

  // CASE 1: Barrier
  if (is_mpi_barrier(region)) {
    // Receive max( all BEGIN event timestamps )
    amortization_data ex_object;
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                  CLC_MAX, event->getComm()->getComm());

    clc_location ex_loc;
    timestamp_t sendEvtT = unpack_data(ex_loc, ex_object);
    fa_amortize_recv(event, sendEvtT, ex_loc);
  }

  // CASE 2: N-to-N communication
  else if (is_mpi_n2n(region)) {
    // Ignore MPI_Alltoallv and MPI_Alltoallw...
    MpiCollEnd_rep&           endRep = event_cast<MpiCollEnd_rep>(*event);
    MpiCollEnd_rep::coll_type type   = endRep.getCollType();
    if ((type == MpiCollEnd_rep::ALLTOALLV) ||
        (type == MpiCollEnd_rep::ALLTOALLW))
      return;

    // ... and locations sending no data
    if (event->getBytesSent() == 0)
      local_object.timestamp = -DBL_MAX;

    // Receive max( all BEGIN event timestamps )
    amortization_data ex_object;
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                  CLC_MAX, event->getComm()->getComm());

    // Ignore locations receiving no data
    if (event->getBytesReceived() == 0)
      ex_object.timestamp = -DBL_MAX;

    clc_location ex_loc;
    timestamp_t sendEvtT = unpack_data(ex_loc, ex_object);
    fa_amortize_recv(event, sendEvtT, ex_loc);
  }

  // CASE 3: 1-to-N communication
  else if (is_mpi_12n(region)) {
    // Ignore locations sending no data
    if (event->getBytesSent() == 0)
      local_object.timestamp = -DBL_MAX;

    // Send/receive latest BEGIN event timestamp
    MPI_Bcast(&local_object, 1, AMORT_DATA,
              event->getRoot(),
              event->getComm()->getComm());

    if (isOnRoot(event, m_rank)) {
      fa_amortize_intern(event);
    } else {
      // Ignore locations receiving no data
      if (event->getBytesReceived() == 0)
        local_object.timestamp = -DBL_MAX;

      clc_location ex_loc;
      timestamp_t sendEvtT = unpack_data(ex_loc, local_object);
      fa_amortize_recv(event, sendEvtT, ex_loc);
    }
  }

  // CASE 4: N-to-1 communication
  else if (is_mpi_n21(region)) {
    // Ignore locations sending no data
    if (event->getBytesSent() == 0)
      local_object.timestamp = -DBL_MAX;

    // Receive max ( all BEGIN event times + latency )
    amortization_data ex_object;
    MPI_Reduce(&local_object, &ex_object, 1, AMORT_DATA, CLC_MAX,
               event->getRoot(),
               event->getComm()->getComm());

    if (isOnRoot(event, m_rank)) {
      // Ignore locations receiving no data
      if (event->getBytesReceived() == 0)
        ex_object.timestamp = -DBL_MAX;

      clc_location ex_loc;
      timestamp_t sendEvtT = unpack_data(ex_loc, ex_object);
      fa_amortize_recv(event, sendEvtT, ex_loc);
    } else {
      fa_amortize_intern(event);
    }
  }

  // CASE 5: Prefix reduction
  else if (is_mpi_scan(region)) {
    // Ignore locations sending no data
    if (event->getBytesSent() == 0)
      local_object.timestamp = -DBL_MAX;

    // Receive latest BEGIN event timestamp
    amortization_data ex_object;
    MPI_Scan(&local_object, &ex_object, 1, AMORT_DATA, CLC_MAX,
              event->getComm()->getComm());

    // Ignore locations receiving no data
    if (event->getBytesReceived() == 0)
      ex_object.timestamp = -DBL_MAX;

    clc_location ex_loc;
    timestamp_t sendEvtT = unpack_data(ex_loc, ex_object);
    fa_amortize_recv(event, sendEvtT, ex_loc);
  }

  // We should never reach this point, but just in case...
  else {
    fa_amortize_intern(event);
  }

  // No further amortization necessary
  FwdAmortData* data = static_cast<FwdAmortData*>(cdata);
  data->setAmortized();
}


void Synchronizer::amortize_fwd_fork(const CallbackManager& cbmanager,
                                     int user_event, const Event& event,
                                     CallbackData* cdata)
{
#ifdef _OPENMP
  // Apply controlled logical clock (internal amortization)
  fa_amortize_intern(event);

  // Exchange fork event timestamp
  omp_global_max_timestamp = event->getTimestamp();

  // Increase nesting level
  omp_global_nest_level++;

  // No further amortization necessary
  FwdAmortData* data = static_cast<FwdAmortData*>(cdata);
  data->setAmortized();
#endif   // _OPENMP
}


void Synchronizer::amortize_fwd_join(const CallbackManager& cbmanager,
                                     int user_event, const Event& event,
                                     CallbackData* cdata)
{
#ifdef _OPENMP
  cur_event_type = CLC_OMP;
  fa_amortize_recv(event, omp_global_max_timestamp, m_loc);

  // Decrease nesting level
  omp_global_nest_level--;

  // No further amortization necessary
  FwdAmortData* data = static_cast<FwdAmortData*>(cdata);
  data->setAmortized();
#endif   // _OPENMP
}


void Synchronizer::amortize_fwd_ttb(const CallbackManager& cbmanager,
                                    int user_event, const Event& event,
                                    CallbackData* cdata)
{
#ifdef _OPENMP
  #pragma omp barrier
  cur_event_type = CLC_OMP;
  fa_amortize_recv(event, omp_global_max_timestamp, m_loc);

  // No further amortization necessary
  FwdAmortData* data = static_cast<FwdAmortData*>(cdata);
  data->setAmortized();
#endif   // _OPENMP
}


void Synchronizer::amortize_fwd_tte(const CallbackManager& cbmanager,
                                    int user_event, const Event& event,
                                    CallbackData* cdata)
{
#ifdef _OPENMP
  fa_amortize_intern(event);
  // Exchange THREAD_TEAM_END event timestamp
  OMP_Allreduce_max(event->getTimestamp(), omp_global_max_timestamp);

  // No further amortization necessary
  FwdAmortData* data = static_cast<FwdAmortData*>(cdata);
  data->setAmortized();
#endif   // _OPENMP
}


void Synchronizer::amortize_fwd_leave(const CallbackManager& cbmanager,
                                      int user_event, const Event& event,
                                      CallbackData* cdata)
{
#ifdef _OPENMP
  const Region& region = event->getRegion();
  if (!is_omp_barrier(region))
    return;

  // Skip OpenMP barrier handling when running serially
  // (the barriers called in the algorithm cause ill side effects...)
  if (0 == omp_global_nest_level)
    return;

  Event enter = event.enterptr();
  OMP_Allreduce_max(enter->getTimestamp(), omp_global_max_timestamp);
  cur_event_type = CLC_OMP;
  fa_amortize_recv(event, omp_global_max_timestamp, m_loc);

  // No further amortization necessary
  FwdAmortData* data = static_cast<FwdAmortData*>(cdata);
  data->setAmortized();
#endif   // _OPENMP
}


/**
 *  Amortization for receive events. Including both replays.
 */
void Synchronizer::fa_amortize_recv(const Event& event,
                                    timestamp_t sendEvtT,
                                    clc_location& ex_loc)
{
  // Timestamp before any amortization
  timestamp_t curT = event->getTimestamp();

  // Amortize forward
  if(firstEvtT == -DBL_MAX) {
    // Store first event timestamp for use in backwards amortization
    firstEvtT = curT;

    // Set initial clock value
    m_clc->set_value(firstEvtT - m_clc->get_delta());

    // Set initial previous event 
    m_clc->set_prev(firstEvtT - m_clc->get_delta());
  }

  // Timestamp after internal forward amortization
  timestamp_t latency = m_network->get_latency(ex_loc, cur_event_type);
  timestamp_t internT = m_clc->amortize_forward_recv(curT, sendEvtT, latency, cur_event_type);

  // Set new timestamp
  event->setTimestamp(m_clc->get_value());

  // If violation detected store event
  if (internT < m_clc->get_value())
    m_violations[event] = internT;

  // Adjust statistics
  r_stat->set_statistics(curT, m_clc->get_value());
}


/**
 *  Amortization for internal events. Only Controlled logical clock is
 *  locally applied.
 */
void Synchronizer::fa_amortize_intern(const Event& event)
{
  // Timestamp before any amortization
  timestamp_t curT = event->getTimestamp();
  if(firstEvtT == -DBL_MAX) {
      // Store first event timestamp for use in backwards amortization
      firstEvtT = curT;

      // Set initial clock value
      m_clc->set_value(curT);

      // Set initial previous event 
      m_clc->set_prev(curT);
  } else {
      // Amortize timestamp internally
      m_clc->amortize_forward_intern(curT);
  }

  // Set new timestamp
  event->setTimestamp(m_clc->get_value());

  // Adjust statistics and timestamp
  r_stat->set_statistics(curT, m_clc->get_value());
}


void Synchronizer::amortize_bkwd_send(const CallbackManager& cbmanager,
                                      int user_event, const Event& event,
                                      CallbackData* cdata)
{
  // Receive receive-event timestamp
  amortization_data ex_object;
  timestamp_t ex_time;
  clc_location ex_loc;

  MPI_Status stat;
  MPI_Recv(&ex_object, 1, AMORT_DATA, event->getDestination(),
           event->getTag(), event->getComm()->getComm(), &stat);

  // Unpack data received
  ex_time = unpack_data(ex_loc, ex_object);

  // Adjust data for local backward amortization
  tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_P2P);
  num_sends++;
}


void Synchronizer::amortize_bkwd_recv(const CallbackManager& cbmanager,
                                      int user_event, const Event& event,
                                      CallbackData* cdata)
{
  // Send receive-event time 
  MPI_Request request;
  amortization_data *local = new amortization_data;
  *local = pack_data(m_loc, event->getTimestamp());

  MPI_Isend(local, 1, AMORT_DATA, event->getSource(),
            event->getTag(), event->getComm()->getComm(), &request);
  m_requests.push_back(request);
  m_data.push_back(local);
  update_requests();
}


void Synchronizer::amortize_bkwd_coll(const CallbackManager& cbmanager,
                                      int user_event, const Event& event,
                                      CallbackData* cdata)
{
  // Determine corresponding ENTER event, region, and communicator
  Event         enter  = event.enterptr();
  const Region& region = enter->getRegion();
  MpiComm*      comm   = event->getComm();

  // Pack local data
  amortization_data local_object = pack_data(m_loc, event->getTimestamp());

  // CASE 1: Barrier
  if (is_mpi_barrier(region)) {
    // Send/recv min (all LEAVE event times)
    amortization_data ex_object;
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                  CLC_MIN, comm->getComm());

    // Unpack data received
    clc_location ex_loc;
    timestamp_t ex_time = unpack_data(ex_loc, ex_object);

    // Adjust data for local backward amortization
    tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_COLL);

    num_sends++;
  }

  // CASE 2: N-to-N communication
  else if (is_mpi_n2n(region)) {
    // Ignore locations receiving no data
    if (event->getBytesReceived() == 0)
      local_object.timestamp = DBL_MAX;

    // Send/recv min (all LEAVE event times)
    amortization_data ex_object;
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
                  CLC_MIN, comm->getComm());

    // Unpack data received
    clc_location ex_loc;
    timestamp_t ex_time = unpack_data(ex_loc, ex_object);

    // Adjust data for local backward amortization
    tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_COLL);

    // Ignore locations sending no data
    if (event->getBytesSent() == 0)
      tsa[num_sends] = DBL_MAX;

    num_sends++;
  }

  // CASE 3: 1-to-N communication
  else if (is_mpi_12n(region)) {
    // Ignore locations receiving no data
    if (event->getBytesReceived() == 0)
      local_object.timestamp = DBL_MAX;

    // Send/recv min (all LEAVE event times)
    amortization_data ex_object;
    MPI_Reduce(&local_object, &ex_object, 1, AMORT_DATA, CLC_MIN,
               event->getRoot(),
               comm->getComm());

    // Adjust data for local backward amortization (only on root)
    if(isOnRoot(event, m_rank)) {
      clc_location ex_loc;
      timestamp_t ex_time;
      ex_time        = unpack_data(ex_loc, ex_object);
      tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_COLL);

      // Ignore locations sending no data
      if (event->getBytesSent() == 0)
        tsa[num_sends] = DBL_MAX;

      num_sends++;
    }
  }

  // CASE 4: N-to-1 communication
  else if (is_mpi_n21(region)) {
    // Ignore locations receiving no data
    if (event->getBytesReceived() == 0)
      local_object.timestamp = DBL_MAX;

    // Send/recv root LEAVE event time 
    MPI_Bcast(&local_object, 1, AMORT_DATA,
              event->getRoot(),
              comm->getComm());

    // Adjust data for local backward amortization
    if (!isOnRoot(event, m_rank)) {
      clc_location ex_loc;
      timestamp_t ex_time;
      ex_time        = unpack_data(ex_loc, local_object);
      tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_COLL);

      // Ignore locations sending no data
      if (event->getBytesSent() == 0)
        tsa[num_sends] = DBL_MAX;

      num_sends++;
    }
  }

  // CASE 5: Partial reduction
  else if (is_mpi_scan(region)) {
    // Ignore locations receiving no data
    if (event->getBytesReceived() == 0)
      local_object.timestamp = DBL_MAX;

    // Create "reverse" communicator
    MPI_Comm reversed_comm;
    MPI_Comm_split (comm->getComm(), 42,
                    (comm->getGroup().numRanks() - comm->getGroup().getLocalRank(event.get_location().getRank()) -1),
                    &reversed_comm);

    // Receive earliest LEAVE event timestamp
    amortization_data ex_object;
    MPI_Scan(&local_object, &ex_object, 1, AMORT_DATA, CLC_MIN, reversed_comm);

    // Destroy "reverse" communicator
    MPI_Comm_free(&reversed_comm);

    // Unpack data received
    clc_location ex_loc;
    timestamp_t ex_time;
    ex_time = unpack_data(ex_loc, ex_object);

    // Adjust data for local backward amortization
    tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_COLL);

    // Ignore locations sending no data
    if (event->getBytesSent() == 0)
      tsa[num_sends] = DBL_MAX;

    num_sends++;
  }

  // We should never reach the (missing) 'else' part -- but if we do,
  // it would be a no-op anyway...
}


void Synchronizer::amortize_bkwd_fork(const CallbackManager& cbmanager,
                                      int user_event, const Event& event,
                                      CallbackData* cdata)
{
#ifdef _OPENMP
  // Decrease nesting level (backward replay!)
  omp_global_nest_level--;

  // Send receive-event time - latency
  // Adjust data for local backward amortization
  clc_location ex_loc;
  tsa[num_sends] = omp_global_max_timestamp - m_network->get_latency(ex_loc, CLC_OMP);
  num_sends++;
#endif   // _OPENMP
}


void Synchronizer::amortize_bkwd_join(const CallbackManager& cbmanager,
                                      int user_event, const Event& event,
                                      CallbackData* cdata)
{
#ifdef _OPENMP
  // Increase nesting level (backward replay!)
  omp_global_nest_level++;

  // Send receive-event time - latency
  clc_location ex_loc;
  omp_global_max_timestamp = event->getTimestamp() - m_network->get_latency(ex_loc, CLC_OMP);
#endif   // _OPENMP
}


void Synchronizer::amortize_bkwd_tte(const CallbackManager& cbmanager,
                                     int user_event, const Event& event,
                                     CallbackData* cdata)
{
#ifdef _OPENMP
  // Adjust data for local backward amortization
  { // FCCpx requires block
    #pragma omp barrier
  }
  clc_location ex_loc;
  tsa[num_sends] = omp_global_max_timestamp - m_network->get_latency(ex_loc, CLC_OMP);
  num_sends++;
#endif   // _OPENMP
}


void Synchronizer::amortize_bkwd_leave(const CallbackManager& cbmanager,
                                       int user_event, const Event& event,
                                       CallbackData* cdata)
{
#ifdef _OPENMP
  const Region& region = event->getRegion();
  if(!is_omp_barrier(region))
    return;

  // Skip OpenMP barrier handling when running serially
  // (the barriers called in the algorithm cause ill side effects...)
  if (0 == omp_global_nest_level)
    return;

  // Receive and send earliest barrier LEAVE event timestamp
  OMP_Allreduce_min(event->getTimestamp(), omp_global_min_timestamp);

  // Adjust data for local backward amortization
  clc_location ex_loc;
  tsa[num_sends] = omp_global_min_timestamp - m_network->get_latency(ex_loc, CLC_OMP);
  num_sends++;
#endif   // _OPENMP
}


/**
 *  Store original timestamps in buffer.
 */
void Synchronizer::get_ts()
{
  long i = 0;
  uint32_t num = trace.num_events();
  tsa = new timestamp_t[num];

  LocalTrace::iterator it = trace.begin();
  while (it != trace.end()) {
    tsa[i] = it->getTimestamp();
    ++i;
    ++it;
  }
}


/**
 *  Restore original timestamps stored by get_ts() before.
 */
void Synchronizer::set_ts()
{
  int i = 0;

  LocalTrace::iterator it = trace.begin();
  while (it != trace.end()) {
    it->setTimestamp(tsa[i]);
    ++i;
    ++it;
  }
}


/**
 *  Print some fancy runtime statistics :-).
 */
void Synchronizer::print_statistics()
{
  timestamp_t gl_corr       = r_stat->get_num_clock_corr();
  timestamp_t gl_max_error  = r_stat->get_max_clock_error();
  timestamp_t gl_error      = r_stat->get_clock_error();
  timestamp_t gl_num        = 0.0;
  timestamp_t num           = (timestamp_t) trace.num_events ();
  timestamp_t gl_violations = 0.0;
  timestamp_t violations    = (timestamp_t) m_clc->get_num_viol();
  timestamp_t gl_max_slope     = 0.0;

  int sum_p2p_violation     = 0;
  int p2p_violation         = m_clc->get_p2p_viol();
  int sum_coll_violation    = 0;
  int coll_violation        = m_clc->get_coll_viol();
  int sum_omp_violation     = 0;
  int omp_violation         =m_clc->get_omp_viol();

#ifdef _OPENMP
  OMP_Allreduce_sum(num, omp_global_sum_timestamp);
  num = omp_global_sum_timestamp;
  OMP_Allreduce_sum(violations, omp_global_sum_timestamp);
  violations = omp_global_sum_timestamp;
  OMP_Allreduce_max(m_max_slope, omp_global_max_timestamp);
  m_max_slope = omp_global_max_timestamp;
  OMP_Allreduce_sum(p2p_violation, omp_global_sum_count);
  p2p_violation = omp_global_sum_count;
  OMP_Allreduce_sum(coll_violation, omp_global_sum_count);
  coll_violation = omp_global_sum_count;
  OMP_Allreduce_sum(omp_violation, omp_global_sum_count);
  omp_violation = omp_global_sum_count;
#endif   // _OPENMP

  #pragma omp master
  {
    MPI_Allreduce(&num, &gl_num, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&violations, &gl_violations, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&m_max_slope, &gl_max_slope, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&p2p_violation, &sum_p2p_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&coll_violation, &sum_coll_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&omp_violation, &sum_omp_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // Print statistics only on master rank
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (0 == rank) {
      printf("\n\t# passes        : %d\n", r_stat->get_pass());
      printf("\t# violated      : %.0f\n", gl_violations);
      if (gl_violations > 0.0) {
        printf("\t# corrected     : %.0f\n", gl_corr);
        printf("\t# reversed-p2p  : %d\n", sum_p2p_violation);
        printf("\t# reversed-coll : %d\n", sum_coll_violation);
        printf("\t# reversed-omp  : %d\n", sum_omp_violation);
        printf("\t# events        : %.0f\n", gl_num);
        printf("\tmax. error      : %.6f [s]\n", gl_max_error);
        printf("\terror at final. : %.6f [%%] \n", gl_error*100);
        printf("\tMax slope       : %.9f  \n", gl_max_slope);
      }
    }
  }
}


/**
 *  Applies backward amortization
 *  First : Replay
 *  Second: Local Correction
 **/
void Synchronizer::ba_amortize()
{
  LocalTrace::iterator it = trace.begin();
  map<Event,timestamp_t,EventCompare>::iterator iter;

  // Set up callback manager
  CallbackManager* cbmanager = new CallbackManager;
  cbmanager->register_callback(GROUP_SEND, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_send));
  cbmanager->register_callback(GROUP_RECV, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_recv));
  cbmanager->register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_coll));
#ifdef _OPENMP
  cbmanager->register_callback(THREAD_FORK, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_fork));
  cbmanager->register_callback(THREAD_JOIN, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_join));
  cbmanager->register_callback(THREAD_TEAM_END, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_tte));
  cbmanager->register_callback(LEAVE, PEARL_create_callback(this, &Synchronizer::amortize_bkwd_leave));
#endif   // _OPENMP

  // Perform backward replay to obtain (earliest) receive timestamp
  PEARL_backward_replay(trace, *cbmanager, 0);

  // Release callback manager
  delete cbmanager;

  // Apply local correction
  // Index of next send event
  int mySend = num_sends - 1;

  // Traverse the trace from begin to end. 
  while (it != trace.end()) {
    // For every receive event R encountered along the way with d_r > 0 
    iter = m_violations.find(it);
    if (iter != m_violations.end()) {
      // Calculate delta_r
      timestamp_t       recv_s    = iter->first->getTimestamp();
      timestamp_t       recv_o    = iter->second;
      timestamp_t       delta_r   = recv_s - recv_o;

      // Start the piece-wise linear amortization
      // with Er := R = iter->first
      //      tr := r* = r - d_r = recv_o
      //      delta_er := delta_r
      //      slope m := const = max_error = 0.01
      ba_piecewise_interpolation(iter->first, recv_o, delta_r, max_error, mySend);
    }

    if (isLogicalSend(it, m_rank)) {
      mySend--;
    }
    ++it;
  }
}


/**
 *  Backward amortization for receive events.
 *  Computes an interval, L_A, and amortizes events on this interval
 *  using a piecewise linear interpolation function 
 **/
void Synchronizer::ba_piecewise_interpolation (const Event& event_rhs,
                                               timestamp_t time_rhs,
                                               timestamp_t delta_rhs,
                                               timestamp_t slope_g1,
                                               long send_idx)
{
  // Index of the preceding send event 
  // Used to obtain correcponding receive event timestamp in timestamp array
  int prec_send_idx  = send_idx + 1;

  // Left corner tl := tr - (delta_rhs/m). 
  // Calculate left corner of the amortization interval 
  timestamp_t m_left = time_rhs - (delta_rhs / slope_g1);

  // If tl < begin of the trace e_b then tl := e_b and m := delta_rhs/(tr - e_b).
  // with tr := r* = r - d_r = time_rhs  
  // Set or (if left_t < firstEvtT) calculate m_slope_g1
  timestamp_t m_slope = slope_g1;

  if (m_left < firstEvtT) {
    m_left  = firstEvtT;
    m_slope = delta_rhs / (time_rhs - m_left);
  }

  // Linear function g1(t) := m * t - (m * tl) = m_slope * t + m_const
  // Calculate offset of linear equation
  timestamp_t m_const = -1.0 * m_slope * m_left;

  // Search for the send event Si in [tl,Er) 
  // with slope mi := (delta_rhs - delta_i)/(tr - si) maximal 
  // and mi > m.

  // Up til now we have not found such a send event
  bool found_send = false;

  // Declare individuelle offsets and slopes at distinct send events
  timestamp_t delta_i, slope_i;

  // Temporary variables to store max. values during the search phase
  Event       event_send = event_rhs;
  timestamp_t slope_send = 0.0;
  timestamp_t delta_send = 0.0;
  timestamp_t const_send = 0.0;
  int         index_send = 0;

  for (Event it = event_rhs.prev(); it.is_valid() && m_left < it->getTimestamp() ; --it) {
    // Send event found?
    if (isLogicalSend(it, m_rank)) {
      // Determine offset between send and receive (-l_min)
      delta_i = tsa[prec_send_idx] - it->getTimestamp();
      //if (it->getTimestamp() > tsa[prec_send_idx] ) 
      // Calculate individuelle slope
      slope_i = (delta_rhs - delta_i)/(time_rhs - it->getTimestamp());

      // If individual slope is larger than initial slope
      if (slope_i > m_slope) {
        // Found conflicting send event
        found_send = true;

        // Store new maximum slope
        m_slope    = slope_i;

        // Store event data of send event
        event_send = it;
        delta_send = delta_i;
        slope_send = slope_i;
        const_send = delta_send - (slope_send * event_send->getTimestamp());
        index_send = prec_send_idx;
      }

      // Increment pointer to preceding send
      prec_send_idx++;
    }
  }

  // If there is no such Si, apply the linear amortization to the interval [tl,Er) 
  // and continue the forward traversal 

  if (!found_send) {
    ba_linear_amortization_time_to_event(m_left, event_rhs , m_slope, m_const);
  } else {
    // Apply the linear amortization to the interval [si,Er) using gi
    ba_linear_amortization_event_to_event(event_send, event_rhs, slope_send, const_send);

    // If delta_i > 0 start the piece-wise linear amortization again 
    // with Er := Si = event_send
    // tr := si
    // delta_rhs := delat_s
    // m = delta_i/(si - tl).
    time_rhs   = event_send->getTimestamp() - delta_send;
    slope_send = delta_send / (time_rhs - m_left);
    if (delta_send > 0.0)
      ba_piecewise_interpolation(event_send, time_rhs, delta_send, slope_send, index_send);
  }
}


void Synchronizer::ba_linear_amortization_event_to_event(Event& event_lhs, const Event& event_rhs, 
                                                         timestamp_t slope_g, timestamp_t c_g)
{
  for(Event it = event_rhs.prev(); it.is_valid() && event_lhs.prev() != it; --it) {
    it->setTimestamp( it->getTimestamp() + (slope_g * it->getTimestamp()) + c_g);
  }
  if (m_max_slope < slope_g)
    m_max_slope = slope_g;
}


void Synchronizer::ba_linear_amortization_time_to_event(timestamp_t time_lhs, const Event& event_rhs, 
                                                        timestamp_t slope_g, timestamp_t c_g)
{
  for(Event it = event_rhs.prev(); it.is_valid() && time_lhs < it->getTimestamp(); --it) {
    it->setTimestamp(it->getTimestamp() + (slope_g * it->getTimestamp()) + c_g);
  }
}


void Synchronizer::update_requests()
{
  // No pending requests? ==> continue
  if (m_requests.empty())
    return;

  // Check for completed messages
  int completed;
  int count = m_requests.size();
  m_indices.resize(count);
  m_statuses.resize(count);

  MPI_Testsome(count, &m_requests[0], &completed, &m_indices[0], &m_statuses[0]);

  m_requests.erase(remove(m_requests.begin(), m_requests.end(),
                          static_cast<MPI_Request>(MPI_REQUEST_NULL)),
                   m_requests.end());

  // Update array of messages
  for (int i = 0; i < completed; ++i) {
    int index = m_indices[i];
    delete m_data[index];
    m_data[index] = 0;
  }
  m_data.erase(remove(m_data.begin(), m_data.end(),
                      static_cast<amortization_data*>(0)),
               m_data.end());
}


void Synchronizer::delete_requests()
{
  MPI_Status status;

  update_requests();

  // Handle remaining messages
  if (!m_requests.empty()) {
    int count = m_requests.size();

    // Print warning
    elg_warning("Encountered %d unreceived send operations!", count);

    // Cancel pending messages
    for (int i = 0; i < count; ++i) {
      MPI_Cancel(&m_requests[i]);
      MPI_Wait(&m_requests[i], &status);
      delete m_data[i];
    }

    m_requests.clear();
    m_data.clear();
  }
}
