/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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
#include "OmpEventHandler.h"

#include "Callstack.h"
#include "CbData.h"
#include "user_events.h"

#include <pearl/CallbackManager.h>
#include <pearl/Event.h>

#include <omp.h>

#include <set>

using namespace scout;
using namespace pearl;
using namespace std;


struct OmpEventHandler::OmpEvtHImpl
{
    //
    // --- private data
    //

    typedef std::map<pearl::Event, pearl::timestamp_t, const pearl::EventKeyCompare> 
      TimeMap;
    typedef std::map<Event, CollectiveInfo, EventKeyCompare> 
      CollinfoMap;

    std::set<Event, const EventKeyCompare> mBarriers;
    TimeMap                                mForkJoinTimes;
    CollinfoMap                            mCollInfo;

    //
    // --- query interface
    //

    pearl::timestamp_t getForkJoinTime(const Event& e) const {
        TimeMap::const_iterator it = mForkJoinTimes.find(e);

        assert(it != mForkJoinTimes.end());

        return it->second;
    }


    //
    // --- main replay callbacks
    //

    SCOUT_CALLBACK(cb_pre_omp_forkjoin) {
        CbData* data = static_cast<CbData*>(cdata);

        mForkJoinTimes.insert(make_pair(event, data->mIdle));
    }

    SCOUT_CALLBACK(cb_pre_omp_barrier) {
        CbData* data = static_cast<CbData*>(cdata);

        TimeRank        my;

        my.time = data->mCallstack->top()->getTimestamp();
        my.rank = omp_get_thread_num();

        static TimeRank latest;

#pragma omp barrier
        { }
#pragma omp master
        {
            latest.time = 0;
            latest.rank = -1;
        }
#pragma omp barrier
        { }
#pragma omp critical
        {
            if (latest.time < my.time)
                latest = my;
        }
#pragma omp barrier
        { }

        CollectiveInfo ci;

        ci.my     = my;
        ci.latest = latest;

        mCollInfo.insert(make_pair(event, ci));
        mBarriers.insert(event);
    }

    //
    // --- backward delay/critical-path analysis callbacks (bwc)
    //

    SCOUT_CALLBACK(cb_pre_enter) {        
        // Check if this is a previously registered parallel region enter
        if (mForkJoinTimes.count(event) > 0)
            cbmanager.notify(OMP_MGMT_FORK, event, cdata);
    }

    SCOUT_CALLBACK(cb_pre_leave) {
        // Check if this is a previously registered parallel region leave
        // or barrier
        if (mForkJoinTimes.count(event) > 0)
            cbmanager.notify(OMP_MGMT_JOIN, event, cdata);
        else if (mBarriers.count(event) > 0) {
            CbData* data = static_cast<CbData*>(cdata);

            CollinfoMap::iterator it = mCollInfo.find(event);

            assert(it != mCollInfo.end());
            data->mCollinfo = it->second;

            cbmanager.notify(OMP_BARRIER, event, cdata);
        }
    }

    SCOUT_CALLBACK(cb_post_enter) {        
        // Check if this is a previously registered parallel region enter
        if (mForkJoinTimes.count(event) > 0)
            cbmanager.notify(OMP_MGMT_POST_FORK, event, cdata);
    }

    //
    // --- callback registration
    //

    void
    register_pre_pattern_callbacks(const CallbackManagerMap& cbm)
    {
        struct cb_evt_table_t
        {
            pearl::event_t evt;
            void           (OmpEvtHImpl::* cbp)(const pearl::CallbackManager &, int, const pearl::Event &, pearl::CallbackData*);
        };
        struct cb_uevt_table_t
        {
            int  evt;
            void (OmpEvtHImpl::* cbp)(const pearl::CallbackManager &, int, const pearl::Event &, pearl::CallbackData*);
        };

        // --- main forward replay callbacks

        const struct cb_evt_table_t main_evt_tbl[] = {
            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t main_uevt_tbl[] = {
#ifdef _OPENMP
            { OMP_MGMT_FORK, &OmpEvtHImpl::cb_pre_omp_forkjoin },
            { OMP_MGMT_JOIN, &OmpEvtHImpl::cb_pre_omp_forkjoin },
            { OMP_EBARRIER,  &OmpEvtHImpl::cb_pre_omp_barrier  },
            { OMP_IBARRIER,  &OmpEvtHImpl::cb_pre_omp_barrier  },
#endif
            { 0, 0 }
        };

        // --- backward wait-state / synchpoint detection callbacks

        const struct cb_evt_table_t bws_evt_tbl[] = {
            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t bws_uevt_tbl[] = {
            { 0, 0 }
        };

        // --- forward synchpoint detection callbacks

        const struct cb_evt_table_t fws_evt_tbl[] = {
#ifdef _OPENMP
            { ENTER, &OmpEvtHImpl::cb_pre_enter },
            { LEAVE, &OmpEvtHImpl::cb_pre_leave },
#endif

            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t fws_uevt_tbl[] = {
            { 0, 0 }
        };

        // --- delay / critical-path analysis backward replay (bwc) callbacks

        const struct cb_evt_table_t bwc_evt_tbl[] = {
#ifdef _OPENMP
            { ENTER, &OmpEvtHImpl::cb_pre_enter },
            { LEAVE, &OmpEvtHImpl::cb_pre_leave },
#endif
            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t bwc_uevt_tbl[] = {
            { 0, 0 }
        };

        // --- global callback tables table :)

        const struct cb_tables_t
        {
            const char*                   stage;
            const struct cb_evt_table_t*  evts;
            const struct cb_uevt_table_t* uevts;
        }
        cb_tables[] = {
            { "",    main_evt_tbl, main_uevt_tbl },
            { "bws", bws_evt_tbl,  bws_uevt_tbl  },
            { "fws", fws_evt_tbl,  fws_uevt_tbl  },
            { "bwc", bwc_evt_tbl,  bwc_uevt_tbl  },

            {     0,            0,             0 }
        };

        // --- register callbacks

        for (const struct cb_tables_t* t = cb_tables; t->stage; ++t) {
            CallbackManagerMap::const_iterator it = cbm.find(t->stage);

            assert(it != cbm.end());

            for (const struct cb_evt_table_t* c = t->evts; c->cbp; ++c) {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
            for (const struct cb_uevt_table_t* c = t->uevts; c->cbp; ++c) {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
        }
    }

    void
    register_post_pattern_callbacks(const CallbackManagerMap& cbm)
    {
        struct cb_evt_table_t
        {
            pearl::event_t evt;
            void           (OmpEvtHImpl::* cbp)(const pearl::CallbackManager &, int, const pearl::Event &, pearl::CallbackData*);
        };
        struct cb_uevt_table_t
        {
            int  evt;
            void (OmpEvtHImpl::* cbp)(const pearl::CallbackManager &, int, const pearl::Event &, pearl::CallbackData*);
        };

        // --- main forward replay callbacks

        const struct cb_evt_table_t main_evt_tbl[] = {
            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t main_uevt_tbl[] = {
            { 0, 0 }
        };

        // --- backward wait-state / synchpoint detection callbacks

        const struct cb_evt_table_t bws_evt_tbl[] = {
            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t bws_uevt_tbl[] = {
            { 0, 0 }
        };

        // --- forward synchpoint detection callbacks

        const struct cb_evt_table_t fws_evt_tbl[] = {
#ifdef _OPENMP
            { ENTER, &OmpEvtHImpl::cb_post_enter },
#endif
            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t fws_uevt_tbl[] = {
            { 0, 0 }
        };

        // --- delay / critical-path analysis backward replay (bwc) callbacks

        const struct cb_evt_table_t bwc_evt_tbl[] = {
            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t bwc_uevt_tbl[] = {
            { 0, 0 }
        };

        // --- global callback tables table :)

        const struct cb_tables_t
        {
            const char*                   stage;
            const struct cb_evt_table_t*  evts;
            const struct cb_uevt_table_t* uevts;
        }
        cb_tables[] = {
            { "",    main_evt_tbl, main_uevt_tbl },
            { "bws", bws_evt_tbl,  bws_uevt_tbl  },
            { "fws", fws_evt_tbl,  fws_uevt_tbl  },
            { "bwc", bwc_evt_tbl,  bwc_uevt_tbl  },

            {     0,            0,             0 }
        };

        // --- register callbacks

        for (const struct cb_tables_t* t = cb_tables; t->stage; ++t) {
            CallbackManagerMap::const_iterator it = cbm.find(t->stage);

            assert(it != cbm.end());

            for (const struct cb_evt_table_t* c = t->evts; c->cbp; ++c) {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
            for (const struct cb_uevt_table_t* c = t->uevts; c->cbp; ++c) {
                (it->second)->register_callback(c->evt, PEARL_create_callback(this, c->cbp));
            }
        }
    }
};


OmpEventHandler::OmpEventHandler()
    : AnalysisHandler(),
      mP(new OmpEvtHImpl)
{
}

OmpEventHandler::~OmpEventHandler()
{
}

pearl::timestamp_t 
OmpEventHandler::getForkJoinTime(const pearl::Event& e) const
{
    return mP->getForkJoinTime(e);
}

void
OmpEventHandler::register_pre_pattern_callbacks(const CallbackManagerMap& cbm)
{
    mP->register_pre_pattern_callbacks(cbm);
}

void
OmpEventHandler::register_post_pattern_callbacks(const CallbackManagerMap& cbm)
{
    mP->register_post_pattern_callbacks(cbm);
}
