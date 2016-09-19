/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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

#include "MpiDelayOps.h"

#include "CbData.h"
#include "DelayOps.h"
#include "Roles.h"
#include "SynchpointHandler.h"
#include "TmapCacheHandler.h"

#include <pearl/GlobalDefs.h>
#include <pearl/MpiComm.h>
#include <pearl/ProcessGroup.h>

#include <mpi.h>

#include <algorithm>
#include <map>
#include <vector>

using namespace pearl;
using namespace scout;

using std::vector;
using std::map;

// #define DELAYCOLLDBG

#ifdef DELAYCOLLDBG
#include <sstream>
#include <fstream>
#endif


///@brief Get synchpoint computation times & total propagating wait time

static TimeMapSum GetSynchpointTimes(const Event& from,
                                     const Event& to,
                                     const SynchpointHandler* SpH,
                                     const TmapCacheHandler*  TmH)
{
    TimeMapSum     w   = CalculateWaitMap(SpH->getSynchpointsBetween(from, to));
    SynchpointInfo spi = SpH->getSynchpointInfo(from);
    TimeMapSum     m;

    m = PruneDelayTimemap(TmH->getRuntimes(from, to), w.mMap, from, spi);
    m.mSum = w.mSum;

    return m;
}


///@brief Transfer data for collective delay calculation

static void TransferData(const TimeRank&             my,
                         const TimeRank&             root,
                         const TimeMapSum&           groupTmap,
                         const map<int, TimeMapSum>& prevSpTmaps,
                         vector<int>&                cpIdVec,
                         vector<double>&             timeVec,
                         int&                        prevSp,
                         double&                     propWait,
                         MpiComm*                    comm)
{
    // --- setup

    vector<int> prevSpVec(comm->getGroup().numRanks(), 0);
    int         cpVecSize = 0;

    if (my.rank == root.rank) {
        for (map<int, TimeMapSum>::const_iterator it = prevSpTmaps.begin(); it != prevSpTmaps.end(); ++it)
            prevSpVec[it->first] = 1;

        cpIdVec.resize(groupTmap.mMap.size());
        timeVec.resize(groupTmap.mMap.size());

        int i = 0;
        for (timemap_t::const_iterator it = groupTmap.mMap.begin(); it != groupTmap.mMap.end(); ++it) {
            cpIdVec[i] = it->first;
            timeVec[i] = it->second;
            ++i;
        }

        propWait  = groupTmap.mSum;
        cpVecSize = groupTmap.mMap.size();
    }

    // --- transfer

    // who was synched after group synchpoint?
    MPI_Scatter(&prevSpVec[0], 1, MPI_INT,
                &prevSp,       1, MPI_INT, root.rank, comm->getComm());

    // propagating wait time
    MPI_Bcast(&propWait,  1, MPI_DOUBLE, root.rank, comm->getComm());
    // group callpath ID map and times
    MPI_Bcast(&cpVecSize, 1, MPI_INT,    root.rank, comm->getComm());

    if (cpIdVec.size() == 0)
        cpIdVec.resize(cpVecSize);
    if (timeVec.size() == 0)
        timeVec.resize(cpVecSize);

    MPI_Bcast(&cpIdVec[0], cpVecSize, MPI_INT,    root.rank, comm->getComm());
    MPI_Bcast(&timeVec[0], cpVecSize, MPI_DOUBLE, root.rank, comm->getComm());

    // point-to-point exchange for ranks after group synchpoint
    if (my.rank == root.rank) {
        vector<double> vec(cpVecSize);

        for (map<int, TimeMapSum>::const_iterator it = prevSpTmaps.begin(); it != prevSpTmaps.end(); ++it) {
            MPI_Send(const_cast<double*>(&(it->second.mSum)), 1, MPI_DOUBLE,
                     it->first, 0, comm->getComm());

            int i = 0;
            const timemap_t& gtmap(groupTmap.mMap);
            for (timemap_t::const_iterator git = gtmap.begin(); git != gtmap.end(); ++git, ++i) {
                timemap_t::const_iterator pspit = it->second.mMap.find(git->first);
                vec[i] = (pspit == it->second.mMap.end() ? 0.0 : pspit->second);
            }

            MPI_Send(&vec[0], cpVecSize, MPI_DOUBLE, it->first, 0, comm->getComm());
        }
    } else if (prevSp > 0) {
        MPI_Recv(&propWait,   1,         MPI_DOUBLE, root.rank, 0,
                 comm->getComm(), MPI_STATUS_IGNORE);
        MPI_Recv(&timeVec[0], cpVecSize, MPI_DOUBLE, root.rank, 0,
                 comm->getComm(), MPI_STATUS_IGNORE);
    }
}


/// @brief Calculate sumscales and maxscales

static void CalculateScales(const TimeRank&        my,
                            const TimeRank&        root,
                            const map<int, Event>& prevSps,
                            bool                   prevSync,
                            MpiComm*               comm,
                            CbData*                data)
{
    data->mSumScales.clear();
    data->mMaxScales.clear();

    const vector<double>::size_type N = data->mInputScales.size();

    vector<double> sum_in(N);
    vector<double> max_in(N);

    map<uint32_t, double>::const_iterator inputIt;
    vector<double>::size_type             i = 0;

    for (inputIt = data->mInputScales.begin(), i = 0; inputIt != data->mInputScales.end(); ++inputIt, ++i) {
        sum_in[i] = inputIt->second;
        max_in[i] = prevSync ? 0.0 : inputIt->second;
    }

    vector<double> sum_out(N);
    vector<double> max_out(N);

    MPI_Reduce(&sum_in[0], &sum_out[0], N, MPI_DOUBLE, MPI_SUM, root.rank, comm->getComm());
    MPI_Reduce(&max_in[0], &max_out[0], N, MPI_DOUBLE, MPI_MAX, root.rank, comm->getComm());

    if (my.rank == root.rank) {
        vector<ScaleMap> scincmap(N);

        for (inputIt  = data->mInputScales.begin(), i = 0;
             inputIt != data->mInputScales.end();
             ++inputIt, ++i) {
            ScaleMap maxscales;
            maxscales.insert(make_pair(data->mPrevSp, max_out[i]));
            data->mMaxScales.insert(make_pair(inputIt->first, maxscales));
        }

        for (map<int, Event>::const_iterator it = prevSps.begin(); it != prevSps.end(); ++it) {
            vector<double> s(N);

            MPI_Recv(&s[0], N, MPI_DOUBLE, it->first, 0, comm->getComm(), MPI_STATUS_IGNORE);

            for (inputIt  = data->mInputScales.begin(), i = 0;
                 inputIt != data->mInputScales.end();
                 ++inputIt, ++i) {
                data->mMaxScales[inputIt->first].insert(std::make_pair(it->second, max_out[i]));

                max_out[i]               = std::max(max_out[i], s[i]);
                sum_out[i]              -= s[i];
                scincmap[i][it->second] += s[i];
            }
        }

        for (inputIt = data->mInputScales.begin(), i = 0; inputIt != data->mInputScales.end(); ++inputIt, ++i) {
            ScaleMap sumscales;
            sumscales.insert(std::make_pair(data->mPrevSp, sum_out[i]));

            for (ScaleMap::const_iterator it = scincmap[i].begin(); it != scincmap[i].end(); ++it) {
                sumscales.insert(std::make_pair(it->first, sum_out[i]));
                sum_out[i] += it->second;
            }

            data->mSumScales.insert(make_pair(inputIt->first, sumscales));
        }
    } else if (prevSync > 0) {
        MPI_Send(&sum_in[0], N, MPI_DOUBLE, root.rank, 0, comm->getComm());
    }
}


namespace scout
{

/// @brief Get delay analysis timemap for MPI synchronization interval

timemap_t
GetMpiDelayTimemap(const Event&             event,
                   int                      worldrank,
                   const SynchpointHandler* SpH,
                   const TmapCacheHandler*  TmH)
{
    Event             prevSp    = SpH->findPreviousMpiSynchpoint(event, worldrank);
    SynchpointInfoMap propSpMap = SpH->getSynchpointsBetween(prevSp, event);
    TimeMapSum        waitMap   = CalculateWaitMap(propSpMap);

    TimeMapSum m =
        PruneDelayTimemap(TmH->getRuntimes(prevSp, event),
                          waitMap.mMap,
                          prevSp,
                          SpH->getSynchpointInfo(prevSp));

    return m.mMap;
}


/// @brief Calculate delay costs and scaling factors for latearrival collectives

void CalculateCollectiveDelay(const TimeRank& my,
                              const TimeRank& root,
                              const Event&    event,
                              CbData*         data)
{
    MpiComm*            comm  = event->getComm();
    const ProcessGroup& group = comm->getGroup();

    // shortcuts
    const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
    const SynchpointHandler* SpH = data->mSynchpointHandler;

    // synchpoints since last group synchpoint
    map<int, Event>      prevSps;
    // group synchpoint computation times and total prop. wait time
    TimeMapSum           groupTmap;
    // computation times and prop. wait time for synchpoints after group synchpoint
    map<int, TimeMapSum> prevSpTmaps;
    // local timemap of non-root processes
    timemap_t            localTmap;

    bool                 isSynchpoint = SpH->isSynchpoint(event);

#ifdef DELAYCOLLDBG
    static std::ofstream dbg;

    if (!dbg.is_open()) {
        std::ostringstream filename;
        filename << "delaycolldbg_r" << my.rank;
        dbg.open(filename.str().c_str());
    }
#endif

    // --- setup

    if (isSynchpoint) {
        if (my.rank == root.rank) {
            data->mPrevSp =
                SpH->findPreviousMpiGroupSynchpoints(event,
                                                     group,
                                                     group.getGlobalRank(root.rank),
                                                     data->mDefs->numLocationGroups(),
                                                     prevSps);
            data->mPropSpMap =
                SpH->getSynchpointsBetween(data->mPrevSp, event);

            groupTmap = GetSynchpointTimes(data->mPrevSp, event, SpH, TmH);

            for (map<int, Event>::const_iterator it = prevSps.begin(); it != prevSps.end(); ++it)
                prevSpTmaps.insert(std::make_pair(it->first,
                                                  GetSynchpointTimes(it->second, event, SpH, TmH)));
        } else {
            localTmap = GetMpiDelayTimemap(event, group.getGlobalRank(root.rank), SpH, TmH);
        }
    }

    // --- data exchange

    vector<int>    cpIdVec; // vector of call-path ids
    vector<double> diffTimeVec; // vector of call-path times

    int            isPrevSync = 0;
    double         propWait   = 0.0;

    TransferData(my, root, groupTmap, prevSpTmaps, cpIdVec, diffTimeVec, isPrevSync, propWait, comm);

    // --- local delay calculation

    vector<int>::size_type cpVecSize = cpIdVec.size();
    double dsum = 0.0;

    if (isSynchpoint) {
        for (vector<int>::size_type i = 0; i < cpVecSize; ++i) {
            timemap_t::const_iterator fit = localTmap.find(cpIdVec[i]);

            double lt = (fit == localTmap.end() ? 0.0 : fit->second);
            double d  = std::max(diffTimeVec[i] - lt, 0.0);

            diffTimeVec[i]  = d;
            dsum           += d;
        }
    } else {
        std::fill(diffTimeVec.begin(), diffTimeVec.end(), 0.0);
    }

    // --- sum up delays

    data->mCollDelayInfo.clear();
    data->mInputScales.insert(std::make_pair(KEY_WAITTIME, std::max(root.time - my.time, 0.0)));

    for (map<uint32_t, double>::iterator inpIt = data->mInputScales.begin(); inpIt != data->mInputScales.end(); ++inpIt) {
        vector<double> timeVec(diffTimeVec);
        double         scale = dsum;

        // calculate scaling factor and local delays
        if (scale + propWait > 0)
            scale = std::min(inpIt->second / (scale + propWait), 1.0);
        for (int i = 0; i < cpVecSize; ++i)
            timeVec[i] *= scale;

#ifndef MVAPICH2_VERSION
        MPI_Reduce((my.rank == root.rank ? MPI_IN_PLACE : &timeVec[0]),
                   &timeVec[0], cpVecSize, MPI_DOUBLE, MPI_SUM, root.rank, comm->getComm());
#else   // MVAPICH2_VERSION
        // MVAPICH2's MPI_Reduce implementation may fail with a segmentation
        // fault when using MPI_IN_PLACE, root != 0, and all processes in
        // comm residing on the same node.
        vector<double> recvVec;
        recvVec.reserve(cpVecSize);
        MPI_Reduce(&timeVec[0], &recvVec[0], cpVecSize, MPI_DOUBLE, MPI_SUM,
                   root.rank, comm->getComm());
#endif   // MVAPICH2_VERSION

        TimeMapSum delayInfo;

        if (my.rank == root.rank && isSynchpoint) {
            for (vector<int>::size_type i = 0; i < cpVecSize; ++i) {
#ifndef MVAPICH2_VERSION
                delayInfo.mMap[cpIdVec[i]]  = timeVec[i];
                delayInfo.mSum             += timeVec[i];
#else   // MVAPICH2_VERSION
                delayInfo.mMap[cpIdVec[i]]  = recvVec[i];
                delayInfo.mSum             += recvVec[i];
#endif   // MVAPICH2_VERSION
            }
        }

        data->mCollDelayInfo.insert(std::make_pair(inpIt->first, delayInfo));
        inpIt->second = scale;
    }

    // --- get scaling factors for long-term cost and propagating wait-state calculation

    CalculateScales(my, root, prevSps, isPrevSync, comm, data);
}

} // namespace scout
