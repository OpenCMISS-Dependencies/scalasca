/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of EPIK callback functions for definition
 *           records.
 *
 *  This file provides the implementation of EPIK callback functions for
 *  handling global definition records.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "EpikDefCallbacks.h"

#include <string>

#include <elg_readcb.h>

#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>

#include "DefsFactory.h"
#include "EpikArchive.h"
#include "Process.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


//--- Constants -------------------------------------------------------------

// Used for unknown entities
const string str_unknown("");


//--- Constructors & destructor ---------------------------------------------

EpikDefCbData::EpikDefCbData(EpikArchive& archive,
                             GlobalDefs&  defs)
    : mArchive(archive),
      mDefinitions(defs),
      mStringContCount(0),
      mStringContId(PEARL_NO_ID),
      mWorldSize(1)
{
}


//--- Mapping functions -----------------------------------------------------

ident_t
EpikDefCbData::get_file(ident_t id) const
{
    if (id == PEARL_NO_ID) {
        return PEARL_NO_ID;
    }
    if (id >= mFileMap.size()) {
        throw RuntimeError("Oops! Invalid file name ID.");
    }
    return mFileMap[id];
}


//--- Local helper functions ------------------------------------------------

namespace
{
Region::Role
convertRegionRole(const string& name,
                  elg_ui1       type,
                  Paradigm      paradigm,
                  bool          isArtificial)
{
    switch (type) {
        case ELG_FUNCTION:
            if (paradigm == Paradigm::MPI) {
                // Fix role of MPI point-to-point functions
                if ((name == "MPI_Bsend")
                    || (name == "MPI_Bsend_init")
                    || (name == "MPI_Ibsend")
                    || (name == "MPI_Iprobe")
                    || (name == "MPI_Irecv")
                    || (name == "MPI_Irsend")
                    || (name == "MPI_Isend")
                    || (name == "MPI_Issend")
                    || (name == "MPI_Probe")
                    || (name == "MPI_Recv")
                    || (name == "MPI_Recv_init")
                    || (name == "MPI_Rsend")
                    || (name == "MPI_Rsend_init")
                    || (name == "MPI_Send")
                    || (name == "MPI_Send_init")
                    || (name == "MPI_Sendrecv")
                    || (name == "MPI_Sendrecv_replace")
                    || (name == "MPI_Ssend")
                    || (name == "MPI_Ssend_init")
                    || (name == "MPI_Start")
                    || (name == "MPI_Startall")) {
                    return Region::ROLE_POINT2POINT;
                }
                // Fix role of MPI one-sided operations
                else if ((name == "MPI_Put") || (name == "MPI_Get")) {
                    return Region::ROLE_RMA;
                }
                else if (name == "MPI_Accumulate") {
                    return Region::ROLE_ATOMIC;
                }
                // Fix role of MPI memory operations
                else if (name == "MPI_Alloc_mem") {
                    return Region::ROLE_ALLOCATE;
                }
                else if (name == "MPI_Free_mem") {
                    return Region::ROLE_DEALLOCATE;
                }
            }

            // Fix role of OpenMP API functions
            else if (paradigm == Paradigm::OPENMP) {
                if ((name == "omp_destroy_lock")
                    || (name == "omp_destroy_nest_lock")
                    || (name == "omp_get_dynamic")
                    || (name == "omp_get_max_threads")
                    || (name == "omp_get_nested")
                    || (name == "omp_get_num_procs")
                    || (name == "omp_get_num_threads")
                    || (name == "omp_get_thread_num")
                    || (name == "omp_in_parallel")
                    || (name == "omp_init_lock")
                    || (name == "omp_init_nest_lock")
                    || (name == "omp_set_dynamic")
                    || (name == "omp_set_lock")
                    || (name == "omp_set_nest_lock")
                    || (name == "omp_set_nested")
                    || (name == "omp_set_num_threads")
                    || (name == "omp_test_lock")
                    || (name == "omp_test_nest_lock")
                    || (name == "omp_unset_lock")
                    || (name == "omp_unset_nest_lock")) {
                    return Region::ROLE_WRAPPER;
                }
            }
            return Region::ROLE_FUNCTION;

        case ELG_LOOP:
            return Region::ROLE_LOOP;

        case ELG_USER_REGION:
            return Region::ROLE_CODE;

        case ELG_FUNCTION_COLL_BARRIER:
            return Region::ROLE_BARRIER;

        case ELG_FUNCTION_COLL_ONE2ALL:
            return Region::ROLE_COLL_ONE2ALL;

        case ELG_FUNCTION_COLL_ALL2ONE:
            return Region::ROLE_COLL_ALL2ONE;

        case ELG_FUNCTION_COLL_ALL2ALL:
            return Region::ROLE_COLL_ALL2ALL;

        case ELG_FUNCTION_COLL_OTHER:
            return Region::ROLE_COLL_OTHER;

        case ELG_OMP_PARALLEL:
            return Region::ROLE_PARALLEL;

        case ELG_OMP_LOOP:
            return Region::ROLE_LOOP;

        case ELG_OMP_SECTIONS:
            return Region::ROLE_SECTIONS;

        case ELG_OMP_SECTION:
            return Region::ROLE_SECTION;

        case ELG_OMP_WORKSHARE:
            return Region::ROLE_WORKSHARE;

        case ELG_OMP_SINGLE:
            return Region::ROLE_SINGLE;

        case ELG_OMP_MASTER:
            return Region::ROLE_MASTER;

        case ELG_OMP_CRITICAL:
            return Region::ROLE_CRITICAL;

        case ELG_OMP_ATOMIC:
            return Region::ROLE_ATOMIC;

        case ELG_OMP_BARRIER:
            return Region::ROLE_BARRIER;

        case ELG_OMP_IBARRIER:
            return Region::ROLE_IMPLICIT_BARRIER;

        case ELG_OMP_FLUSH:
            return Region::ROLE_FLUSH;

        case ELG_OMP_CRITICAL_SBLOCK:
            return Region::ROLE_CRITICAL_SBLOCK;

        case ELG_OMP_SINGLE_SBLOCK:
            return Region::ROLE_SINGLE_SBLOCK;

        case ELG_OMP_ORDERED:
            return Region::ROLE_ORDERED;

        case ELG_OMP_ORDERED_SBLOCK:
            return Region::ROLE_ORDERED_SBLOCK;

        default:
            if (isArtificial) {
                return Region::ROLE_ARTIFICIAL;
            }
            break;
    }

    return Region::ROLE_UNKNOWN;
}


Paradigm
convertParadigm(const string& descr,
                const string& name)
{
    if (descr == "USR") {
        return Paradigm::USER;
    }
    if (descr == "OMP" || descr == "POMP") {
        return Paradigm::OPENMP;
    }
    if (descr == "MPI") {
        return Paradigm::MPI;
    }
    if (descr == "EPIK") {
        if (name == "PAUSING") {
            return Paradigm::USER;
        }
        return Paradigm::MEASUREMENT_SYSTEM;
    }
    return Paradigm::UNKNOWN;
}
}   // unnamed namespace


//--- Local helper macros ---------------------------------------------------

#define CONV_ID(id)      (((id) == ELG_NO_ID) ? PEARL_NO_ID : (id))
#define CONV_NUM(num)    (((num) == ELG_NO_NUM) ? PEARL_NO_NUM : (num))

#define CALLBACK_SETUP                                            \
    EpikDefCbData * data = static_cast<EpikDefCbData*>(userdata); \
    GlobalDefs& defs = data->mDefinitions;                        \
    try {
#define CALLBACK_SETUP_NO_DEFS                                    \
    EpikDefCbData * data = static_cast<EpikDefCbData*>(userdata); \
    try {
#define CALLBACK_CLEANUP                 \
    } /* Closes the try block */         \
    catch (exception& ex) {              \
        data->mErrorMessage = ex.what(); \
    }

//--- EPIK: Generic callbacks -----------------------------------------------

void
elg_readcb_ALL(elg_ui1 type,          /* record type */
               elg_ui1 length,        /* record length */
               void*   userdata)
{
    /* The type and length fields are ignored during reading. */
}


//--- EPIK: Global definition callbacks -------------------------------------

void
elg_readcb_STRING(elg_ui4     strid,        /* string identifier */
                  elg_ui1     cntc,         /* number of continuation records */
                  const char* str,          /* string */
                  void*       userdata)
{
    CALLBACK_SETUP

    // Partial definition => Save attributes
    if (cntc != 0) {
        data->mStringContId    = strid;
        data->mStringContCount = cntc;
        data->mStringContData  = string(str, 250);
        return;
    }

    DefsFactory::instance()->createString(defs, strid, str);

    CALLBACK_CLEANUP
}


void
elg_readcb_STRING_CNT(const char* str,        /* continued string */
                      void*       userdata)
{
    CALLBACK_SETUP

    // Sanity check
    if (data->mStringContCount == 0) {
        throw RuntimeError("Oops! Unexpected ELG_STRING_CNT record.");
    }

    // Partial definition => Append string fragment
    data->mStringContCount--;
    if (data->mStringContCount > 0) {
        data->mStringContData.append(str, 255);
        return;
    }

    // Append final fragment and create definition
    data->mStringContData.append(str);
    DefsFactory::instance()->createString(defs,
                                          data->mStringContId,
                                          data->mStringContData);
    data->mStringContData.clear();

    CALLBACK_CLEANUP
}


void
elg_readcb_MACHINE(elg_ui4 mid,          /* machine identifier */
                   elg_ui4 nodec,        /* number of nodes */
                   elg_ui4 mnid,         /* machine name identifier */
                   void*   userdata)
{
    CALLBACK_SETUP

    uint32_t nodeId = defs.numSystemNodes();
    data->mMachineMap[mid] = nodeId;
    DefsFactory::instance()->createSystemNode(defs,
                                              nodeId,
                                              CONV_ID(mnid),
                                              String::MACHINE_ID,
                                              SystemNode::NO_ID);

    CALLBACK_CLEANUP
}


void
elg_readcb_NODE(elg_ui4 nid,         /* node identifier */
                elg_ui4 mid,         /* machine identifier */
                elg_ui4 cpuc,        /* number of CPUs */
                elg_ui4 nnid,        /* node name identifer */
                elg_d8  cr,          /* number of clock cycles/sec */
                void*   userdata)
{
    CALLBACK_SETUP

    uint32_t nodeId = defs.numSystemNodes();
    data->mNodeMap[nid] = nodeId;
    DefsFactory::instance()->createSystemNode(defs,
                                              nodeId,
                                              CONV_ID(nnid),
                                              String::NODE_ID,
                                              data->mMachineMap[mid]);

    CALLBACK_CLEANUP
}


void
elg_readcb_PROCESS(elg_ui4 pid,         /* process identifier */
                   elg_ui4 pnid,        /* process name identifier */
                   void*   userdata)
{
    CALLBACK_SETUP

    data->mProcessMap[CONV_ID(pid)] = CONV_ID(pnid);

    CALLBACK_CLEANUP
}


void
elg_readcb_THREAD(elg_ui4 tid,         /* thread identifier */
                  elg_ui4 pid,         /* process identifier */
                  elg_ui4 tnid,        /* thread name identifier */
                  void*   userdata)
{
    CALLBACK_SETUP

    data->mThreadMap[
        map < pair < ident_t, ident_t >, ident_t > ::key_type(CONV_ID(pid), CONV_ID(tid))
    ] = CONV_ID(tnid);

    CALLBACK_CLEANUP
}


void
elg_readcb_LOCATION(elg_ui4 lid,        /* location identifier */
                    elg_ui4 mid,        /* machine identifier */
                    elg_ui4 nid,        /* node identifier */
                    elg_ui4 pid,        /* process identifier */
                    elg_ui4 tid,        /* thread identifier */
                    void*   userdata)
{
    CALLBACK_SETUP

    uint32_t                              pnid = String::NO_ID;
    map<ident_t, ident_t>::const_iterator pit  = data->mProcessMap.find(CONV_ID(pid));
    if (pit != data->mProcessMap.end()) {
        pnid = pit->second;
    }
    try {
        defs.getLocationGroup(CONV_ID(pid));
    }
    catch (RuntimeError& ex) {
        DefsFactory::instance()->createLocationGroup(defs,
                                                     CONV_ID(pid),
                                                     pnid,
                                                     LocationGroup::TYPE_PROCESS,
                                                     data->mNodeMap[CONV_ID(nid)]);
        const LocationGroup& group   = defs.getLocationGroup(CONV_ID(pid));
        Process&             process = const_cast<Process&>(static_cast<const Process&>(group));
        process.setRank(CONV_ID(pid));
    }

    // Find thread name string definition
    uint32_t                                             tnid = String::NO_ID;
    map<pair<ident_t, ident_t>, ident_t>::const_iterator tit  =
        data->mThreadMap.find(
            map<pair<ident_t, ident_t>, ident_t>::key_type(CONV_ID(pid), CONV_ID(tid)));
    if (tit != data->mThreadMap.end()) {
        tnid = tit->second;
    }

    // Calculate location ID and store mapping
    Location::IdType locId = (static_cast<Location::IdType>(tid) << 32) + pid;
    data->mArchive.addIdMapping(lid, locId);

    // Create location instance
    DefsFactory::instance()->createLocation(defs,
                                            locId,
                                            tnid,
                                            Location::TYPE_CPU_THREAD,
                                            0,
                                            CONV_ID(pid));

    CALLBACK_CLEANUP
}


void
elg_readcb_FILE(elg_ui4 fid,         /* file identifier */
                elg_ui4 fnid,        /* file name identifier */
                void*   userdata)
{
    CALLBACK_SETUP_NO_DEFS

    // Sanity check
    if (fid != data->mFileMap.size()) {
        throw RuntimeError("Oops! Invalid file name ID.");
    }

    // Store file name in callback data structure
    data->mFileMap.push_back(CONV_ID(fnid));

    CALLBACK_CLEANUP
}


void
elg_readcb_REGION(elg_ui4 rid,          /* region identifier */
                  elg_ui4 rnid,         /* region name identifier */
                  elg_ui4 fid,          /* source file identifier */
                  elg_ui4 begln,        /* begin line number */
                  elg_ui4 endln,        /* end line number */
                  elg_ui4 rdid,         /* region description identifier */
                  elg_ui1 rtype,        /* region type */
                  void*   userdata)
{
    CALLBACK_SETUP

    uint32_t fnameId = data->get_file(fid);
    string   name    = defs.getString(CONV_ID(rnid)).getString();
    string   desc    = defs.getString(CONV_ID(rdid)).getString();
    string   file    = defs.getString(fnameId).getString();

    // Handle special "PAUSING" and "TRACING" regions
    DefsFactory::InternalRegionType type         = DefsFactory::USER_REGION;
    bool                            isArtificial = false;
    if (("EPIK" == desc) && ("EPIK" == file)) {
        isArtificial = true;
        if ("PAUSING" == name) {
            type = DefsFactory::PAUSING_REGION;
        } else if ("TRACING" == name) {
            type = DefsFactory::FLUSHING_REGION;
        }
    }

    Paradigm paradigm = convertParadigm(desc, name);
    DefsFactory::instance()->createRegion(defs,
                                          CONV_ID(rid),
                                          CONV_ID(rnid),
                                          CONV_ID(rnid),
                                          CONV_ID(rdid),
                                          convertRegionRole(name,
                                                            rtype,
                                                            paradigm,
                                                            isArtificial),
                                          paradigm,
                                          fnameId,
                                          CONV_NUM(begln),
                                          CONV_NUM(endln),
                                          type);

    CALLBACK_CLEANUP
}


void
elg_readcb_CALL_SITE(elg_ui4 csid,        /* call site identifier */
                     elg_ui4 fid,         /* source file identifier */
                     elg_ui4 lno,         /* line number */
                     elg_ui4 erid,        /* region identifer to be entered */
                     elg_ui4 lrid,        /* region identifer to be left */
                     void*   userdata)
{
    CALLBACK_SETUP

    DefsFactory::instance()->createCallsite(defs,
                                            CONV_ID(csid),
                                            data->get_file(fid),
                                            CONV_NUM(lno),
                                            CONV_ID(erid));

    CALLBACK_CLEANUP
}


void
elg_readcb_CALL_PATH(elg_ui4 cpid,        /* call-path identifier */
                     elg_ui4 rid,         /* node region identifier */
                     elg_ui4 ppid,        /* parent call-path identifier */
                     elg_ui8 order,       /* node order specifier */
                     void*   userdata)
{
    CALLBACK_SETUP

    DefsFactory::instance()->createCallpath(defs,
                                            CONV_ID(cpid),
                                            CONV_ID(rid),
                                            PEARL_NO_ID,
                                            CONV_ID(ppid));

    CALLBACK_CLEANUP
}


void
elg_readcb_METRIC(elg_ui4 metid,           /* metric identifier */
                  elg_ui4 metnid,          /* metric name identifier */
                  elg_ui4 metdid,          /* metric description identifier */
                  elg_ui1 metdtype,        /* metric data type */
                  elg_ui1 metmode,         /* metric mode */
                  elg_ui1 metiv,           /* time interval referenced */
                  void*   userdata)
{
    CALLBACK_SETUP

    DefsFactory::instance()->createMetric(defs,
                                          CONV_ID(metid),
                                          defs.getString(CONV_ID(metnid)).getString(),
                                          defs.getString(CONV_ID(metdid)).getString(),
                                          metdtype,
                                          metmode,
                                          metiv);

    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_GROUP(elg_ui4 gid,              /* group identifier */
                     elg_ui1 mode,             /* mode flags */
                     elg_ui4 grpc,             /* number of ranks in group */
                     elg_ui4 grpv[],           /* local |-> global rank mapping */
                     void*   userdata)
{
    CALLBACK_SETUP

    // Set up process identifier list
    DefsFactory::process_group process_ids;
    process_ids.reserve(grpc);
    for (elg_ui4 i = 0; i < grpc; ++i) {
        process_ids.push_back(grpv[i]);
    }

    // Create MPI group record
    DefsFactory::instance()->createMpiGroup(defs,
                                            CONV_ID(gid),
                                            String::NO_ID,
                                            process_ids,
                                            mode & ELG_GROUP_SELF,
                                            mode & ELG_GROUP_WORLD);

    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_COMM_DIST(elg_ui4 cid,         /* communicator id unique to root */
                         elg_ui4 root,        /* global rank of root process */
                         elg_ui4 lcid,        /* local communicator id on process */
                         elg_ui4 lrank,       /* local rank of process */
                         elg_ui4 size,        /* size of communicator */
                         void*   userdata)
{
    EpikDefCbData* data = static_cast<EpikDefCbData*>(userdata);

    data->mErrorMessage = "Unexpected ELG_MPI_COMM_DIST record!";
}


void
elg_readcb_MPI_COMM_REF(elg_ui4 cid,           /* communicator identifier */
                        elg_ui4 gid,           /* group identifier */
                        void*   userdata)
{
    CALLBACK_SETUP

    // Create MPI communicator record
    DefsFactory::instance()->createMpiComm(defs,
                                           CONV_ID(cid),
                                           String::NO_ID,
                                           CONV_ID(gid),
                                           Communicator::NO_ID);

    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_COMM(elg_ui4  cid,         /* communicator identifier */
                    elg_ui1  mode,        /* mode flags */
                    elg_ui4  grpc,        /* size of bitstring in bytes */
                    elg_ui1* grpv,        /* bitstring defining the group */
                    void*    userdata)
{
    CALLBACK_SETUP

    // Set up process identifier list
    DefsFactory::process_group process_ids;
    process_ids.reserve(grpc * 8);
    for (uint32_t byte = 0; byte < grpc; ++byte) {
        uint8_t value = grpv[byte];

        for (int bit = 0; bit < 8; ++bit) {
            if (value & (1 << bit)) {
                uint32_t pid = byte * 8 + bit;

                // Store global process identifier
                process_ids.push_back(pid);
            }
        }
    }

    // MPI_COMM_WORLD is always defined by communicator with global ID 0
    if (0 == cid) {
        data->mWorldSize = process_ids.size();
    }

    // Determine WORLD flag
    bool is_world = (data->mWorldSize == process_ids.size());

    // Create MPI group record
    DefsFactory::instance()->createMpiGroup(defs,
                                            CONV_ID(cid),
                                            String::NO_ID,
                                            process_ids,
                                            mode & ELG_GROUP_SELF,
                                            is_world);

    // Create MPI communicator record
    DefsFactory::instance()->createMpiComm(defs,
                                           CONV_ID(cid),
                                           String::NO_ID,
                                           CONV_ID(cid),
                                           Communicator::NO_ID);

    CALLBACK_CLEANUP
}


void
elg_readcb_CART_TOPOLOGY(elg_ui4  topid,          /* topology id */
                         elg_ui4  tnid,           /* topology name id */
                         elg_ui4  cid,            /* communicator id */
                         elg_ui1  ndims,          /* number of dimensions */
                         elg_ui4* dimv,           /* number of processes in each dim */
                         elg_ui1* periodv,        /* periodicity in each dim */
                         elg_ui4* dimids,         /* dimension name ids */
                         void*    userdata)
{
    CALLBACK_SETUP

    // Set up dimension vector
    Cartesian::cart_dims dimensions;
    dimensions.reserve(ndims);
    for (elg_ui1 i = 0; i < ndims; ++i) {
        dimensions.push_back(dimv[i]);
    }

    // Set up periodicity vector
    Cartesian::cart_period periodic;
    periodic.reserve(ndims);
    for (elg_ui1 i = 0; i < ndims; ++i) {
        periodic.push_back(periodv[i]);
    }

    // Create new cartesian grid
    if (cid != ELG_NO_ID) {
        DefsFactory::instance()->createMpiCartesian(defs,
                                                    CONV_ID(topid),
                                                    dimensions,
                                                    periodic,
                                                    CONV_ID(cid));
    } else {
        DefsFactory::instance()->createCartesian(defs,
                                                 CONV_ID(topid),
                                                 dimensions,
                                                 periodic);
    }
    CALLBACK_CLEANUP
}


void
elg_readcb_CART_COORDS(elg_ui4  topid,         /* topology id */
                       elg_ui4  lid,           /* location id */
                       elg_ui1  ndims,         /* number of dimensions */
                       elg_ui4* coordv,        /* coordinates in each dimension */
                       void*    userdata)
{
    CALLBACK_SETUP

    // Retrieve cartesian topology
    Cartesian* cart = defs.get_cartesian(CONV_ID(topid));

    // Set up coordinate vector
    Cartesian::cart_coords coordinate;
    coordinate.reserve(ndims);
    for (elg_ui1 i = 0; i < ndims; ++i) {
        coordinate.push_back(coordv[i]);
    }

    // Set coordinate
    cart->set_coords(defs.getLocation(data->mArchive.mapLocationId(lid)),
                     coordinate);

    CALLBACK_CLEANUP
}


void
elg_readcb_MPI_WIN(elg_ui4 wid,        /* window identifier */
                   elg_ui4 cid,        /* communicator identifier */
                   void*   userdata)
{
    CALLBACK_SETUP

    DefsFactory::instance()->createMpiWindow(defs,
                                             CONV_ID(wid),
                                             CONV_ID(cid));

    CALLBACK_CLEANUP
}


void
elg_readcb_MAP_OFFSET(elg_ui4 rank,        /* rank number the offset relates to */
                      elg_ui4 offset,      /* file offset in mapping file */
                      void*   userdata)
{
    CALLBACK_SETUP

    data->mArchive.setOffset(rank, offset);

    CALLBACK_CLEANUP
}


//--- EPIK: Obsolete definition records -------------------------------------

void
elg_readcb_LAST_DEF(void* userdata)
{
}


void
elg_readcb_NUM_EVENTS(elg_ui4 eventc,        /* number of events */
                      void*   userdata)
{
}


void
elg_readcb_EVENT_TYPES(elg_ui4 ntypes,          /* number of event types */
                       elg_ui1 typev[],         /* vector of event types */
                       void*   userdata)
{
}


void
elg_readcb_EVENT_COUNTS(elg_ui4 ntypes,          /* number of event types */
                        elg_ui4 countv[],        /* vector of event counts */
                        void*   userdata)
{
}
