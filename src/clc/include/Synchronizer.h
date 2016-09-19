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


#ifndef CLC_SYNCHRONIZER_H
#define CLC_SYNCHRONIZER_H


#include <functional>
#include <map>
#include <vector>

#include <mpi.h>

#include <pearl/CallbackData.h>
#include <pearl/CallbackManager.h>
#include <pearl/Event.h>
#include <pearl/LocalTrace.h>
#include <pearl/pearl_types.h>

#include "clc_types.h"


class Clock;
class FwdAmortData;
class IntervalStat;
class Network;
class RuntimeStat;
class TimeStat;
struct amortization_data;


/// Helper class comparing events based on their ID rather than on their
/// timestamps.
struct EventCompare
    : public std::binary_function<pearl::Event, pearl::Event, bool>
{
    bool operator()(const pearl::Event& ev1, const pearl::Event& ev2) const
    {
        return ev1.get_id() < ev2.get_id();
    }
};


/**
 *  Scalable timestamp synchronization class for
 *  postmortem performance analysis.
 *  
 *  Uses the Extended Controlled Logical Clock
 **/
class Synchronizer
{
  public:
    // Constructors and Destructor
    Synchronizer(int rank, const pearl::LocalTrace& myTrace);
    Synchronizer(int rank, const pearl::LocalTrace& myTrace, TimeStat* t_stat, IntervalStat* i_stat);
    ~Synchronizer();

    // Used to synchronize event stream
    void synchronize();
    void forward_amortization();
    void backward_amortization();

    // Statistic functions
    void print_statistics();


  private:
    // Make instances uncopyable
    Synchronizer(const Synchronizer& rhs);
    Synchronizer& operator=(const Synchronizer& rhs);


    // Extended controllel logical clock
    Clock* m_clc;

    // Network the clock is running on
    Network* m_network;

    // Location of the clock
    clc_location m_loc;

    // Statistics
    RuntimeStat* r_stat;

    // Local event trace
    const pearl::LocalTrace& trace;

    // First event's timestamp
    pearl::timestamp_t firstEvtT;

    // Initial slope for the backward amortization
    double max_error;
    pearl::timestamp_t m_max_slope;

    // Threshold for synchronization error at synchronization end
    double threshold;

    // My Rank during the replay
    int m_rank;

    // Number of logical send events
    long num_sends;

    // Local time statistic
    TimeStat* t_stat;

    // Local interval statistic
    IntervalStat* i_stat;

    // Map with the violating receive events
    std::map<pearl::Event, pearl::timestamp_t, EventCompare> m_violations;

    // Timestamp array
    pearl::timestamp_t* tsa;

    // Additional information
    clc_event_t cur_event_type;

    // Asynchronous message handling
    std::vector<amortization_data*> m_data;
    std::vector<MPI_Request>        m_requests;
    std::vector<int>                m_indices;
    std::vector<MPI_Status>         m_statuses;
    void update_requests();
    void delete_requests();

    // Callback functions for amortization
    void amortize_fwd_send (const pearl::CallbackManager& cbmanager, int user_event,
                            const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_fwd_recv (const pearl::CallbackManager& cbmanager, int user_event,
                            const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_fwd_coll (const pearl::CallbackManager& cbmanager, int user_event,
                            const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_fwd_fork (const pearl::CallbackManager& cbmanager, int user_event,
                            const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_fwd_join (const pearl::CallbackManager& cbmanager, int user_event,
                            const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_fwd_ttb  (const pearl::CallbackManager& cbmanager, int user_event,
                            const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_fwd_tte  (const pearl::CallbackManager& cbmanager, int user_event,
                            const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_fwd_leave(const pearl::CallbackManager& cbmanager, int user_event,
                            const pearl::Event& event, pearl::CallbackData* cdata);

    void amortize_bkwd_send (const pearl::CallbackManager& cbmanager, int user_event,
                             const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_bkwd_recv (const pearl::CallbackManager& cbmanager, int user_event,
                             const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_bkwd_coll (const pearl::CallbackManager& cbmanager, int user_event,
                             const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_bkwd_fork (const pearl::CallbackManager& cbmanager, int user_event,
                             const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_bkwd_join (const pearl::CallbackManager& cbmanager, int user_event,
                             const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_bkwd_tte  (const pearl::CallbackManager& cbmanager, int user_event,
                             const pearl::Event& event, pearl::CallbackData* cdata);
    void amortize_bkwd_leave(const pearl::CallbackManager& cbmanager, int user_event,
                             const pearl::Event& event, pearl::CallbackData* cdata);

    // Amortization functions for forward amortization
    void fa_amortize_recv  (const pearl::Event& event,
                            pearl::timestamp_t sendEvtT,
                            clc_location& ex_loc);
    void fa_amortize_intern(const pearl::Event& event);

    // Amortization functions for backward amortization
    void ba_amortize();

    void ba_piecewise_interpolation(const pearl::Event& event_rhs,
                                    pearl::timestamp_t  time_rhs,
                                    pearl::timestamp_t  delta_rhs,
                                    pearl::timestamp_t  slope_g1,
                                    long                send_idx);

    void ba_linear_amortization_event_to_event(pearl::Event&       event_lhs,
                                               const pearl::Event& event_rhs,
                                               pearl::timestamp_t  slope_g,
                                               pearl::timestamp_t  c_g);

    void ba_linear_amortization_time_to_event(pearl::timestamp_t  time_lhs,
                                              const pearl::Event& event_rhs,
                                              pearl::timestamp_t  slope_g,
                                              pearl::timestamp_t  c_g);

    // Useful helper functions
    void init();
    void prepare_replay();
    void get_ts();
    void set_ts();

    friend class FwdAmortData;
};


#endif   // !CLC_SYNCHRONIZER_H
