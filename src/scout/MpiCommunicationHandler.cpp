/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include "MpiCommunicationHandler.h"

#include "CbData.h"
#include "Callstack.h"
#include "SynchpointHandler.h"
#include "Roles.h"
#include "scout_types.h"
#include "user_events.h"

#include <pearl/CallbackManager.h>
#include <pearl/Event.h>
#include <pearl/EventSet.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalData.h>
#include <pearl/MpiCollEnd_rep.h>
#include <pearl/MpiComm.h>
#include <pearl/MpiMessage.h>
#include <pearl/ProcessGroup.h>
#include <pearl/Region.h>
#include <pearl/RemoteData.h>
#include <pearl/RemoteEventSet.h>

#include <epik/elg_error.h>

#include <mpi.h>

#include <algorithm>
#include <cassert>
#include <limits>
#include <map>
#include <vector>

using namespace scout;
using namespace pearl;
using namespace std;

struct MpiCommunicationHandler:: MpiCHImpl
{
    //
    // --- private data --------------------------------------------------
    //

    typedef map<Event, CollectiveInfo, EventKeyCompare> collinfo_map_t;
    typedef map<Communicator::IdType, MpiComm*>         communicator_map_t;

    collinfo_map_t      mCollInfo;

    vector<MpiMessage*> mPendingMsgs;       ///< Pending message objects (main replay)
    vector<MPI_Request> mPendingReqs;       ///< Pending MPI requests (main replay)
    vector<MPI_Status>  mStatuses;
    vector<int>         mIndices;

    communicator_map_t  mInvComms;

    pearl::Event        mInitEnd;
    pearl::Event        mFinalizeEnd;

    ~MpiCHImpl() {
        for (communicator_map_t::iterator it = mInvComms.begin(); it != mInvComms.end(); ++it)
            delete it->second;
    }

    //
    // --- helper functions ----------------------------------------------
    //

    void
    processPendingMsgs()
    {
        // No pending requests? ==> continue
        if (mPendingReqs.empty()) {
            return;
        }

        // --- Check for completed messages

        int completed;
        int count = mPendingReqs.size();

        mIndices.resize(count);
        mStatuses.resize(count);

        MPI_Testsome(count, &mPendingReqs[0], &completed, &mIndices[0], &mStatuses[0]);


        // --- Update array of pending messages

        for (int i = 0; i < completed; ++i) {
            int index = mIndices[i];

            delete mPendingMsgs[index];
            mPendingMsgs[index] = 0;
        }

        if (completed > 0) {
            mPendingMsgs.erase(remove(mPendingMsgs.begin(), mPendingMsgs.end(),
                                      static_cast<MpiMessage*>(0)),
                               mPendingMsgs.end());
            mPendingReqs.erase(remove(mPendingReqs.begin(), mPendingReqs.end(),
                                      static_cast<MPI_Request>(MPI_REQUEST_NULL)),
                               mPendingReqs.end());
        }
    }


    //
    // --- main replay callbacks -----------------------------------------
    //

    SCOUT_CALLBACK(cb_finished) {
        MPI_Barrier(MPI_COMM_WORLD);

        processPendingMsgs();

        // Handle remaining messages
        if (!mPendingReqs.empty()) {
            int count = mPendingReqs.size();

            // Print warning
            elg_warning("Encountered %d unreceived send operations!", count);

            // Cancel pending messages
            for (int i = 0; i < count; ++i) {
                MPI_Cancel(&mPendingReqs[i]);
                mPendingMsgs[i]->wait();
                delete mPendingMsgs[i];
            }
        }

        mPendingMsgs.clear();
        mPendingReqs.clear();
    }


    //
    // --- init/finalize 
    //

    SCOUT_CALLBACK(cb_pre_leave) {
        if (event == mFinalizeEnd || event == mInitEnd) {
            CbData* data = static_cast<CbData*>(cdata);

            collinfo_map_t::iterator it = mCollInfo.find(event);

            assert(it != mCollInfo.end());

            data->mCollinfo = it->second;
          
            cbmanager.notify(event == mInitEnd ? INIT_END : FINALIZE_END, event, data);
        }
    }

    SCOUT_CALLBACK(cb_pre_init) {
        mInitEnd = event.leaveptr();

        CollectiveInfo ci;

        // latest BEGIN
        ci.my.time = event->getTimestamp();
        ci.my.rank = event.get_location().getRank();

        TimeRank trbuf = ci.my;

        MPI_Allreduce(&trbuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);

        // earliest END
        trbuf.time = mInitEnd->getTimestamp();
        trbuf.rank = ci.my.rank;

        MPI_Allreduce(&trbuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

        mCollInfo.insert(make_pair(mInitEnd, ci));
    }

    SCOUT_CALLBACK(cb_pre_finalize) {
        mFinalizeEnd = event.leaveptr();

        CollectiveInfo ci;

        // latest BEGIN
        ci.my.time = event->getTimestamp();
        ci.my.rank = event.get_location().getRank();

        TimeRank trbuf = ci.my;

        MPI_Allreduce(&trbuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);

        // earliest END
        trbuf.time = mFinalizeEnd->getTimestamp();
        trbuf.rank = ci.my.rank;

        MPI_Allreduce(&trbuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

        mCollInfo.insert(make_pair(mFinalizeEnd, ci));
    }


    //
    // --- point-to-point communication
    //

    SCOUT_CALLBACK(cb_process_msgs) {
        processPendingMsgs();
    }

    SCOUT_CALLBACK(cb_pre_send) {
        CbData* data = static_cast<CbData*>(cdata);

        if (!is_mpi_api(data->mCallstack->top()->getRegion())
            || event.completion()->isOfType(MPI_CANCELLED)) {
            return;
        }

        // --- run pre-send callbacks

        cbmanager.notify(PRE_SEND, event, data);

        // --- transfer message

        MpiComm*    comm = event->getComm();
        MpiMessage* msg;

        msg = data->mLocal->isend(*comm, event->getDestination(), event->getTag());

        mPendingMsgs.push_back(msg);
        mPendingReqs.push_back(msg->get_request());

        // --- run post-send callbacks

        cbmanager.notify(POST_SEND, event, data);
    }

    SCOUT_CALLBACK(cb_pre_recv) {
        CbData* data = static_cast<CbData*>(cdata);

        if (!is_mpi_api(data->mCallstack->top()->getRegion())) {
            return;
        }

        // --- run pre-recv callbacks

        cbmanager.notify(PRE_RECV, event, data);

        // --- receive message

        MpiComm* comm = event->getComm();
        data->mRemote->recv(*data->mDefs, *comm, event->getSource(), event->getTag());

        // --- run post-recv callbacks

        cbmanager.notify(POST_RECV, event, data);
    }


    //
    // --- mpi collective communication
    //    

    // --- collective communication dispatch

    SCOUT_CALLBACK(cb_pre_mpi_collend) {
        CbData* data = static_cast<CbData*>(cdata);

        // A single process will not wait for itself
        MpiComm* comm = event->getComm();
        if (comm->getGroup().numRanks() < 2) {
            return;
        }
        Event begin = event.beginptr();
        Event leave = event.leaveptr();
        Event enter = data->mCallstack->top();

        data->mLocal->add_event(enter, ROLE_ENTER_COLL);
        data->mLocal->add_event(begin, ROLE_BEGIN_COLL);
        data->mLocal->add_event(event, ROLE_END_COLL);
        data->mLocal->add_event(leave, ROLE_LEAVE_COLL);

        CollectiveInfo ci;

        ci.my.time = begin->getTimestamp();
        ci.my.rank = comm->getGroup().getLocalRank(event.get_location().getRank());

        data->mCollinfo = ci;

        // Dispatch
        const Region& region = enter->getRegion();
        if (is_mpi_12n(region)) {
            cbmanager.notify(COLL_12N, event, data);
        } else if (is_mpi_n21(region)) {
            cbmanager.notify(COLL_N21, event, data);
        } else if (is_mpi_scan(region)) {
            cbmanager.notify(COLL_SCAN, event, data);
        } else if (is_mpi_barrier(region)) {
            cbmanager.notify(SYNC_COLL, event, data);
        } else if (is_mpi_n2n(region)) {
            // Ignore MPI_Alltoallv and MPI_Alltoallw
            MpiCollEnd_rep&           endRep = event_cast<MpiCollEnd_rep>(*event);
            MpiCollEnd_rep::coll_type type   = endRep.getCollType();
            if ((type == MpiCollEnd_rep::ALLTOALLV)
                || (type == MpiCollEnd_rep::ALLTOALLW)) {
                return;
            }
            cbmanager.notify(COLL_N2N, event, data);
        }

        mCollInfo.insert(make_pair(event, data->mCollinfo));
    }

    // --- mpi collective operation handlers

    SCOUT_CALLBACK(cb_pre_coll_12n) {
        CbData*  data = static_cast<CbData*>(cdata);
        MpiComm* comm = event->getComm();

        CollectiveInfo& ci(data->mCollinfo);

        // distribute root enter time

        ci.root = ci.my;

        MPI_Bcast(&ci.root, 1, MPI_DOUBLE_INT, event->getRoot(), comm->getComm());
    }

    SCOUT_CALLBACK(cb_pre_coll_n21) {
        CbData*  data = static_cast<CbData*>(cdata);
        MpiComm* comm = event->getComm();

        CollectiveInfo& ci(data->mCollinfo);

        // --- root rank and time

        ci.root = ci.my;

        MPI_Bcast(&ci.root, 1, MPI_DOUBLE_INT, event->getRoot(), comm->getComm());

        // --- rank and time of last process (w/o zero-sized transfers)

        TimeRank trbuf = ci.my;

        if (event->getBytesSent() == 0) {
            trbuf.time = numeric_limits<double>::min();
        }
        MPI_Allreduce(&trbuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC, comm->getComm());

        // --- rank and time of earliest process (w/o root and zero-sized transfers)

        if ((event->getBytesSent() == 0) || (ci.my.rank == ci.root.rank)) {
            trbuf.time = numeric_limits<double>::max();
        }
        MPI_Allreduce(&trbuf, &ci.earliest, 1, MPI_DOUBLE_INT, MPI_MINLOC, comm->getComm());

        // --- first exit time

        trbuf.time = event->getTimestamp();
        trbuf.rank = ci.my.rank;

        MPI_Allreduce(&trbuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC, comm->getComm());
    }

    SCOUT_CALLBACK(cb_pre_coll_scan) {
        CbData*  data = static_cast<CbData*>(cdata);
        MpiComm* comm = event->getComm();

        CollectiveInfo& ci(data->mCollinfo);

        // get latest begin of ranks 0..n

        MPI_Scan(&ci.my, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC, comm->getComm());
    }

    SCOUT_CALLBACK(cb_pre_coll_n2n) {
        CbData*  data = static_cast<CbData*>(cdata);
        MpiComm* comm = event->getComm();

        CollectiveInfo& ci(data->mCollinfo);

        uint64_t bytesSent     = event->getBytesSent();
        uint64_t bytesReceived = event->getBytesReceived();

        TimeRank trbuf;

        // latest BEGIN (w/o zero-sized transfers)
        trbuf = ci.my;

        if (bytesSent == 0) {
            trbuf.time = numeric_limits<double>::min();
        }
        MPI_Allreduce(&trbuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC, comm->getComm());

        // earliest END (w/o non-synchronizing processes)
        trbuf.time = event->getTimestamp();
        trbuf.rank = ci.my.rank;

        if ((bytesSent == 0) || (bytesReceived == 0)) {
            trbuf.time = numeric_limits<double>::max();
        }
        MPI_Allreduce(&trbuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC, comm->getComm());
    }

    SCOUT_CALLBACK(cb_pre_sync_coll) {
        CbData*  data = static_cast<CbData*>(cdata);
        MpiComm* comm = event->getComm();

        CollectiveInfo& ci(data->mCollinfo);

        TimeRank trbuf;

        // latest BEGIN
        trbuf = ci.my;

        MPI_Allreduce(&trbuf, &ci.latest, 1, MPI_DOUBLE_INT, MPI_MAXLOC, comm->getComm());

        // earliest END
        trbuf.time = event->getTimestamp();
        trbuf.rank = ci.my.rank;

        MPI_Allreduce(&trbuf, &ci.earliest_end, 1, MPI_DOUBLE_INT, MPI_MINLOC, comm->getComm());
    }


    //
    // --- backward wait-state / synchpoint detection (bws) callbacks ----
    //

    SCOUT_CALLBACK(cb_progress_barrier) {
        // In p2p-only replays (bws, fws, fwc), add artificial barriers 
        // at collective events to prevent p2p overload
        MPI_Barrier(event->getComm()->getComm());
    }

    SCOUT_CALLBACK(cb_bw_pre_send) {
        // backward communication: receive message from original sender

        CbData* data = static_cast<CbData*>(cdata);

        if (!is_mpi_api(event.get_cnode()->getRegion())
            || event.completion()->isOfType(MPI_CANCELLED)) {
            return;
        }

        // --- run pre-recv callbacks

        cbmanager.notify(PRE_SEND, event, data);

        // --- receive message

        MpiComm* comm = event->getComm();
        data->mRemote->recv(*data->mDefs, *comm, event->getDestination(), event->getTag());

        // --- run post-recv callbacks

        cbmanager.notify(POST_SEND, event, data);
    }

    SCOUT_CALLBACK(cb_bw_pre_recv) {
        // backward communication: send message to the original sender

        CbData* data = static_cast<CbData*>(cdata);

        if (!is_mpi_api(event.get_cnode()->getRegion())) {
            return;
        }

        // --- run pre-send callbacks

        cbmanager.notify(PRE_RECV, event, data);

        // --- transfer message

        MpiComm*    comm = event->getComm();
        MpiMessage* msg;

        msg = data->mLocal->isend(*comm, event->getSource(), event->getTag());

        mPendingMsgs.push_back(msg);
        mPendingReqs.push_back(msg->get_request());

        // --- run post-send callbacks

        cbmanager.notify(POST_RECV, event, data);
    }


    //
    // --- forward synchpoint detection (fws) callbacks ------------------
    //

    SCOUT_CALLBACK(cb_fw_pre_send) {
        CbData* data  = static_cast<CbData*>(cdata);

        if (!is_mpi_api(event.get_cnode()->getRegion())
            || event.completion()->isOfType(MPI_CANCELLED)) {
            return;
        }

        // --- run pre-send callbacks

        cbmanager.notify(PRE_SEND, event, data);

        // --- transfer message

        MpiComm*    comm = event->getComm();
        MpiMessage* msg;

        msg = data->mLocal->isend(*comm, event->getDestination(), event->getTag());

        mPendingMsgs.push_back(msg);
        mPendingReqs.push_back(msg->get_request());

        mInvComms.insert(make_pair(comm->getId(), static_cast<MpiComm*>(0)));

        // --- run post-send callbacks

        cbmanager.notify(POST_SEND, event, data);
    }

    SCOUT_CALLBACK(cb_fw_pre_recv) {
        CbData* data = static_cast<CbData*>(cdata);

        if (!is_mpi_api(event.get_cnode()->getRegion()))
            return;

        // --- run pre-recv callbacks

        cbmanager.notify(PRE_RECV, event, data);

        // --- receive message

        MpiComm* comm = event->getComm();
        data->mRemote->recv(*data->mDefs, *comm, event->getSource(), event->getTag());

        mInvComms.insert(make_pair(comm->getId(), static_cast<MpiComm*>(0)));

        // --- run post-recv callbacks

        cbmanager.notify(POST_RECV, event, data);
    }

    SCOUT_CALLBACK(cb_fw_pre_sendcmp) {
        CbData* data  = static_cast<CbData*>(cdata);

        // --- Handle inverse communication.
        // In forward replay, this means msg from receive request to send completion:
        // receive message from receive request here.

        // --- check if this is a late-receiver synchpoint

        if (!data->mSynchpointHandler->isWaitstate(event))
            return;

        // --- receive message

        Event sendevt = event.request();

        communicator_map_t::iterator cit = mInvComms.find(sendevt->getComm()->getId());

        assert(cit != mInvComms.end() && cit->second != 0);

        data->mInvRemote->recv(*data->mDefs,
                               *(cit->second),
                               sendevt->getDestination(),
                               sendevt->getTag());

        // --- run post-recv callbacks

        cbmanager.notify(POST_INV_SENDCMP, event, data);
    }

    SCOUT_CALLBACK(cb_fw_pre_recvreq) {
        CbData* data  = static_cast<CbData*>(cdata);

        // --- Handle inverse communication.
        // In forward replay, this means msg from receive request to send completion:
        // send message to send completion here.

        // --- check if this is a late-receiver synchpoint

        if (!data->mSynchpointHandler->isSynchpoint(event) ||
             data->mSynchpointHandler->isWaitstate(event))
            return;

        // --- run pre-send callbacks

        cbmanager.notify(PRE_INV_RECVREQ, event, data);

        // --- transfer message

        Event recvevt = event.completion();

        communicator_map_t::iterator cit = mInvComms.find(recvevt->getComm()->getId());

        assert(cit != mInvComms.end() && cit->second != 0);

        MpiMessage* msg;

        msg = data->mInvLocal->isend(*(cit->second),
                                     recvevt->getSource(),
                                     recvevt->getTag());

        mPendingMsgs.push_back(msg);
        mPendingReqs.push_back(msg->get_request());
    }


    //
    // --- backward delay / critical-path analysis (bwc) -----------------
    //

    SCOUT_CALLBACK(cb_bwc_prepare) {
        CbData* data  = static_cast<CbData*>(cdata);

        // --- duplicate communicators for inverse communication

        // Find out which communicators were used for p2p

        std::vector<uint32_t> lcomms;

        for (communicator_map_t::const_iterator it = mInvComms.begin(); it != mInvComms.end(); ++it)
            lcomms.push_back(it->first);

        std::vector<uint32_t>::iterator lcit = lcomms.begin();

        for (uint32_t c = 0; c < data->mDefs->numCommunicators(); ++c) {
            uint32_t lid = (lcit == lcomms.end() ? Communicator::NO_ID : *lcit);
            uint32_t gid;

            MPI_Allreduce(&lid, &gid, 1, SCALASCA_MPI_UINT32_T, MPI_MIN, MPI_COMM_WORLD);

            // FIXME: This assumes that Communicator::NO_ID is the max element!
            if (gid == Communicator::NO_ID)
                break;

            // Create communicator duplicate
            const MpiComm& comm = static_cast<const MpiComm&>(data->mDefs->getCommunicator(gid));

            if (comm.getComm() != MPI_COMM_NULL)
                mInvComms[gid] = comm.duplicate();

            if (lcit != lcomms.end() && gid == *lcit)
                ++lcit;
        }
    }

    // --- collective communication dispatch

    SCOUT_CALLBACK(cb_bw_pre_mpi_collend) {
        CbData* data = static_cast<CbData*>(cdata);

        // Set the collective info
        collinfo_map_t::iterator it = mCollInfo.find(event);

        // Return if no collective info found
        // (e.g., in case of < 2 ranks or alltoallv/w)
        if (it == mCollInfo.end())
            return;

        data->mCollinfo = it->second;

        // Dispatch
        const Region& region = event.enterptr()->getRegion();
        if (is_mpi_12n(region)) {
            cbmanager.notify(COLL_12N, event, data);
        } else if (is_mpi_n21(region)) {
            cbmanager.notify(COLL_N21, event, data);
        } else if (is_mpi_scan(region)) {
            cbmanager.notify(COLL_SCAN, event, data);
        } else if (is_mpi_barrier(region)) {
            cbmanager.notify(SYNC_COLL, event, data);
        } else if (is_mpi_n2n(region)) {
            cbmanager.notify(COLL_N2N, event, data);
        }
    }

    SCOUT_CALLBACK(cb_bw_pre_sendcmp) {
        CbData* data  = static_cast<CbData*>(cdata);

        // --- Handle inverse communication. 
        // In backward replay, this means msg from send completion to receive request:
        // send message to receive request here.

        // --- check if this is a late-receiver synchpoint

        if (!data->mSynchpointHandler->isWaitstate(event))
            return;

        // --- run pre-send callbacks

        cbmanager.notify(PRE_INV_SENDCMP, event, data);
  
        // --- transfer message

        Event sendevt = event.request();

        communicator_map_t::iterator cit = mInvComms.find(sendevt->getComm()->getId());

        assert(cit != mInvComms.end() && cit->second != 0);

        MpiMessage* msg;

        msg = data->mInvLocal->isend(*(cit->second),
                                       sendevt->getDestination(),
                                       sendevt->getTag());

        mPendingMsgs.push_back(msg);
        mPendingReqs.push_back(msg->get_request());        
    }

    SCOUT_CALLBACK(cb_bw_pre_recvreq) {
        CbData* data  = static_cast<CbData*>(cdata);

        // --- Handle inverse communication. 
        // In backward replay, this means msg from send completion to receive request:
        // receive message from send completion here.

        // --- check if this is a late-receiver synchpoint

        if (!data->mSynchpointHandler->isSynchpoint(event) ||
             data->mSynchpointHandler->isWaitstate(event))
            return;

        // --- receive message 

        Event recvevt = event.completion();

        communicator_map_t::iterator cit = mInvComms.find(recvevt->getComm()->getId());

        assert(cit != mInvComms.end() && cit->second != 0);

        data->mInvRemote->recv(*data->mDefs,
                               *(cit->second),
                               recvevt->getSource(),
                               recvevt->getTag());

        // --- run post-recv callbacks

        cbmanager.notify(POST_INV_RECVREQ, event, data);
    }

    //
    // --- callback registration -----------------------------------------
    //

    void
    register_pre_pattern_callbacks(const CallbackManagerMap& cbm)
    {
        struct cb_evt_table_t
        {
            pearl::event_t evt;
            void           (MpiCHImpl::* cbp)(const pearl::CallbackManager &, int, const pearl::Event &, pearl::CallbackData*);
        };
        struct cb_uevt_table_t
        {
            int  evt;
            void (MpiCHImpl::* cbp)(const pearl::CallbackManager &, int, const pearl::Event &, pearl::CallbackData*);
        };

        // --- main forward replay callbacks

        const struct cb_evt_table_t main_evt_tbl[] = {
            { MPI_COLLECTIVE_END, &MpiCHImpl::cb_pre_mpi_collend },
            { GROUP_SEND,         &MpiCHImpl::cb_pre_send        },
            { GROUP_RECV,         &MpiCHImpl::cb_pre_recv        },
            { LEAVE,              &MpiCHImpl::cb_pre_leave       },

            { MPI_COLLECTIVE_END, &MpiCHImpl::cb_process_msgs    },
            { GROUP_SEND,         &MpiCHImpl::cb_process_msgs    },
            { GROUP_RECV,         &MpiCHImpl::cb_process_msgs    },
            { GROUP_END,          &MpiCHImpl::cb_process_msgs    },

            { NUM_EVENT_TYPES,    0 }
        };

        const struct cb_uevt_table_t main_uevt_tbl[] = {
            { INIT,               &MpiCHImpl::cb_pre_init        },
            { FINALIZE,           &MpiCHImpl::cb_pre_finalize    },
            { COLL_12N,           &MpiCHImpl::cb_pre_coll_12n    },
            { COLL_N2N,           &MpiCHImpl::cb_pre_coll_n2n    },
            { COLL_N21,           &MpiCHImpl::cb_pre_coll_n21    },
            { COLL_SCAN,          &MpiCHImpl::cb_pre_coll_scan   },
            { SYNC_COLL,          &MpiCHImpl::cb_pre_sync_coll   },

            { FINISHED,           &MpiCHImpl::cb_finished        },

            { 0, 0 }
        };

        // --- backward wait-state / synchpoint detection callbacks

        const struct cb_evt_table_t bws_evt_tbl[] = {
            { LEAVE,              &MpiCHImpl::cb_pre_leave       },

            { GROUP_SEND,         &MpiCHImpl::cb_bw_pre_send     },
            { GROUP_RECV,         &MpiCHImpl::cb_bw_pre_recv     },

            { GROUP_SEND,         &MpiCHImpl::cb_process_msgs    },
            { GROUP_RECV,         &MpiCHImpl::cb_process_msgs    },
            { GROUP_END,          &MpiCHImpl::cb_process_msgs    },

            { MPI_COLLECTIVE_END, &MpiCHImpl::cb_progress_barrier },

            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t bws_uevt_tbl[] = {
            { FINISHED,           &MpiCHImpl::cb_finished        },

            { 0, 0 }
        };

        // --- forward wait-state / synchpoint detection (fws) callbacks

        const struct cb_evt_table_t fws_evt_tbl[] = {
            { LEAVE,              &MpiCHImpl::cb_pre_leave       },

            { GROUP_SEND,         &MpiCHImpl::cb_fw_pre_send     },
            { GROUP_RECV,         &MpiCHImpl::cb_fw_pre_recv     },

            { GROUP_SEND,         &MpiCHImpl::cb_process_msgs    },
            { GROUP_RECV,         &MpiCHImpl::cb_process_msgs    },
            { GROUP_END,          &MpiCHImpl::cb_process_msgs    },

            { MPI_COLLECTIVE_END, &MpiCHImpl::cb_progress_barrier },

            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t fws_uevt_tbl[] = {
            { FINISHED,           &MpiCHImpl::cb_finished        },

            { 0, 0 }
        };

        // --- delay / critical-path analysis backward replay (bwc) callbacks

        const struct cb_evt_table_t bwc_evt_tbl[] = {
            { LEAVE,              &MpiCHImpl::cb_pre_leave       },

            { GROUP_SEND,         &MpiCHImpl::cb_bw_pre_send     },
            { GROUP_RECV,         &MpiCHImpl::cb_bw_pre_recv     },

            { MPI_SEND,           &MpiCHImpl::cb_bw_pre_sendcmp  },
            { MPI_SEND_COMPLETE,  &MpiCHImpl::cb_bw_pre_sendcmp  },
            { MPI_RECV,           &MpiCHImpl::cb_bw_pre_recvreq  },
            { MPI_RECV_REQUEST,   &MpiCHImpl::cb_bw_pre_recvreq  },

            { MPI_COLLECTIVE_END, &MpiCHImpl::cb_bw_pre_mpi_collend },

            { GROUP_SEND,         &MpiCHImpl::cb_process_msgs    },
            { GROUP_RECV,         &MpiCHImpl::cb_process_msgs    },
            { GROUP_END,          &MpiCHImpl::cb_process_msgs    },

            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t bwc_uevt_tbl[] = {
            { PREPARE,            &MpiCHImpl::cb_bwc_prepare     },
            { FINISHED,           &MpiCHImpl::cb_finished        },

            { 0, 0 }
        };

        // --- forward propagating/indirect wait-state analysis (fwc) callbacks

        const struct cb_evt_table_t fwc_evt_tbl[] = {
            { LEAVE,              &MpiCHImpl::cb_pre_leave       },

            { GROUP_SEND,         &MpiCHImpl::cb_fw_pre_send     },
            { GROUP_RECV,         &MpiCHImpl::cb_fw_pre_recv     },

            { MPI_SEND,           &MpiCHImpl::cb_fw_pre_sendcmp },
            { MPI_SEND_COMPLETE,  &MpiCHImpl::cb_fw_pre_sendcmp },
            { MPI_RECV,           &MpiCHImpl::cb_fw_pre_recvreq },
            { MPI_RECV_REQUEST,   &MpiCHImpl::cb_fw_pre_recvreq },

            { GROUP_SEND,         &MpiCHImpl::cb_process_msgs    },
            { GROUP_RECV,         &MpiCHImpl::cb_process_msgs    },
            { GROUP_END,          &MpiCHImpl::cb_process_msgs    },

            { NUM_EVENT_TYPES, 0 }
        };

        const struct cb_uevt_table_t fwc_uevt_tbl[] = {
            { FINISHED,           &MpiCHImpl::cb_finished        },

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
            { "fwc", fwc_evt_tbl,  fwc_uevt_tbl  },

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


MpiCommunicationHandler::MpiCommunicationHandler()
    : AnalysisHandler(),
      mP(new MpiCHImpl)
{
}


MpiCommunicationHandler::~MpiCommunicationHandler()
{
}


void
MpiCommunicationHandler::register_pre_pattern_callbacks(const CallbackManagerMap& cbm)
{
    mP->register_pre_pattern_callbacks(cbm);
}
