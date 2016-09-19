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


#define __STDC_CONSTANT_MACROS

#include <config.h>
#include "ReportWriterTask.h"

#include <cassert>
#include <cstdio>
#include <sstream>
#include <vector>

#if defined(_MPI)
  #include <mpi.h>
#endif   // _MPI

#include <pearl/Callpath.h>
#include <pearl/Callsite.h>
#include <pearl/Cartesian.h>
#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocationGroup.h>
#include <pearl/Region.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>

#include "AnalyzeTask.h"
#include "Logging.h"
#include "ReportData.h"
#include "TaskData.h"

using namespace std;
using namespace pearl;
using namespace scout;


//--- Symbolic names --------------------------------------------------------

#define OUTPUT_FILENAME    "scout"
#define PRIMARY_MIRROR     "http://apps.fz-juelich.de/scalasca/releases/scalasca/2.0/help/"


//---------------------------------------------------------------------------
//
//  class ReportWriterTask
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

/**
 *  @brief Constructor. Creates a new report writer task.
 *
 *  This constructor creates a new report writer task instance using the
 *  given parameters.
 *
 *  @param  sharedData  Shared task data object
 *  @param  analyzer    Analyzer task to query for the severity data
 *  @param  dirname     Experiment archive directory name
 *  @param  rank        Global process rank (MPI rank or 0)
 **/
ReportWriterTask::ReportWriterTask(const TaskDataShared& sharedData,
                                   const AnalyzeTask*    analyzer,
                                   const string&         dirname,
                                   int                   rank)
    : Task(),
      mSharedData(sharedData),
      mAnalyzer(analyzer),
      mDirname(dirname),
      mRank(rank)
{
}


//--- Executing the task ----------------------------------------------------

/**
 *  @brief Executes the report writer task.
 *
 *  The writer task first creates the CUBE output file, writes the static
 *  definition data (meta data) to it, and incrementally collates and writes
 *  the per-pattern severity data.
 *
 *  @return Returns @em true if the execution was successful, @em false
 *          otherwise
 *  @throw  pearl::FatalError  if the CUBE file could not be created.
 **/
bool
ReportWriterTask::execute()
{
    // This variable is implicitly shared!
    static int error = 0;

    ReportData data(*mSharedData.mDefinitions, mRank);
    if (0 == mRank) {
    #pragma omp master
        {
            // Create CUBE object & file
            std::string filename = mDirname + "/" + OUTPUT_FILENAME;
            data.cb = cube_create(const_cast<char*>(filename.c_str()), CUBE_MASTER, CUBE_FALSE);

            // Broadcast whether CUBE file could be opened
            error = (NULL == data.cb) ? 1 : 0;
      #if defined(_MPI)
            MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
      #endif   // _MPI
        }
    #pragma omp barrier
        if (error) {
            throw FatalError("Could not create CUBE report!");
        }
    #pragma omp master
        {
            // Write static definition data
            cube_def_mirror(data.cb, PRIMARY_MIRROR);
            cube_def_attr(data.cb, "CUBE_CT_AGGR", "SUM");

            gen_sysinfo(data);
            gen_regions(data);
            gen_cnodes(data);
            gen_cartesians(data);
            mAnalyzer->genCubeMetrics(data);
        }

        // Collate severity data
        mAnalyzer->genCubeSeverities(data, mRank);

        // Clean up
    #pragma omp master
        {
            cube_free(data.cb);
        }
    } else {
    #pragma omp master
        {
            // Check whether CUBE file could be opened
      #if defined(_MPI)
            MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
      #endif   // _MPI
        }
    #pragma omp barrier
        if (error) {
            throw FatalError("Could not create CUBE report!");
        }

        // Collate severity data
        mAnalyzer->genCubeSeverities(data, mRank);
    }

    return true;
}


//--- Report data generation ------------------------------------------------

/**
 *  @brief Generates the system hierarchy information.
 *
 *  This method defines the CUBE system hierarchy in the CUBE object passed
 *  via @a data based on the information available in the global definitions
 *  object.
 *
 *  @param  data  Report data structure
 *
 *  @todo
 *    - Fix implicit assumption that location group ID == MPI rank
 **/
void
ReportWriterTask::gen_sysinfo(ReportData& data) const
{
    const GlobalDefs& defs = *mSharedData.mDefinitions;

    // Temporary mapping table: system tree node ID |-> CUBE system tree node
    std::vector<cube_system_tree_node*> snodes;

    // Temporary mapping table: location group ID |-> CUBE location group
    std::vector<cube_location_group*> lgroups;

    // Create system tree nodes
    uint32_t count = defs.numSystemNodes();
    for (uint32_t index = 0; index < count; ++index) {
        const SystemNode& snode = defs.getSystemNode(index);

        // Determine parent
        SystemNode*            parent  = snode.getParent();
        cube_system_tree_node* cparent = NULL;
        if (parent) {
            cparent = snodes[parent->getId()];
        }

        // Construct node name
        string name = snode.getClassName().getString() + " "
                      + snode.getName().getString();

        // Create node
        cube_system_tree_node* csnode;
        csnode = cube_def_system_tree_node(data.cb,
                                           name.c_str(),
                                           "",
                                           snode.getClassName().getCString(),
                                           cparent);

        snodes.push_back(csnode);
    }

    // Create location groups
    count = defs.numLocationGroups();
    for (uint32_t index = 0; index < count; ++index) {
        const LocationGroup& lgroup = defs.getLocationGroup(index);

        // Convert location group type
        cube_location_group_type ctype;
        ostringstream            lgname;
        switch (lgroup.getType()) {
            case LocationGroup::TYPE_PROCESS:
                ctype = CUBE_LOCATION_GROUP_TYPE_PROCESS;
                lgname << "MPI rank " << lgroup.getId();
                break;

            default:
                throw RuntimeError("Unknown location group type!");
        }

        // Create location group
        cube_location_group* clgroup;
        clgroup = cube_def_location_group(data.cb,
                                          (lgroup.getName().getString().empty()
                                           ? lgname.str().c_str()
                                           : lgroup.getName().getCString()),
                                          lgroup.getId(),
                                          ctype,
                                          snodes[lgroup.getParent()->getId()]);
        lgroups.push_back(clgroup);

        // Create locations
        uint32_t numLocs = lgroup.numLocations();
        for (uint32_t locIdx = 0; locIdx < numLocs; ++locIdx) {
            const Location& location = lgroup.getLocation(locIdx);

            // Convert location type
            cube_location_type ctype;
            ostringstream      lname;
            switch (location.getType()) {
                case Location::TYPE_CPU_THREAD:
                    ctype = CUBE_LOCATION_TYPE_CPU_THREAD;
                    lname << "CPU thread " << location.getThreadId();
                    break;

                case Location::TYPE_GPU:
                    ctype = CUBE_LOCATION_TYPE_GPU;
                    lname << "GPU " << location.getThreadId();
                    break;

                case Location::TYPE_METRIC:
                    ctype = CUBE_LOCATION_TYPE_METRIC;
                    lname << "metric location " << location.getThreadId();
                    break;

                default:
                    throw RuntimeError("Unknown location type!");
            }

            // Create location
            cube_location* clocation;
            clocation = cube_def_location(data.cb,
                                          (location.getName().getString().empty()
                                           ? lname.str().c_str()
                                           : location.getName().getCString()),
                                          location.getThreadId(),
                                          ctype,
                                          lgroups[location.getParent()->getId()]);

            data.locs[location.getId()] = clocation;
        }
    }
}


/**
 *  @brief Generates the source-code region information.
 *
 *  This method defines the CUBE source-code regions in the CUBE object passed
 *  via @a data based on the information available in the global definitions
 *  object.
 *
 *  @param  data  Report data structure
 *
 *  @todo Add support for canonical names.
 **/
void
ReportWriterTask::gen_regions(ReportData& data) const
{
    const GlobalDefs& defs = *mSharedData.mDefinitions;

    // Create regions
    uint32_t count = defs.numRegions();
    for (ident_t id = 0; id < count; ++id) {
        const Region& region  = defs.getRegion(id);
        cube_region*  cregion = cube_def_region(data.cb,
                                                region.getDisplayName().getCString(),
                                                region.getCanonicalName().getCString(),
                                                pearl::toString(region.getParadigm()).c_str(),
                                                pearl::toString(region.getRole()).c_str(),
                                                region.getStartLine(),
                                                region.getEndLine(),
                                                "",
                                                region.getDescription().getCString(),
                                                region.getFilename().getCString());
        data.regions.push_back(cregion);
    }
}


/**
 *  @brief Generates the call tree information.
 *
 *  This method defines the CUBE call tree in the CUBE object passed via @a
 *  data based on the information available in the global definitions object.
 *
 *  @param  data  Report data structure
 **/
void
ReportWriterTask::gen_cnodes(ReportData& data) const
{
    const GlobalDefs& defs = *mSharedData.mDefinitions;

    // Create call-tree nodes
    uint32_t count = defs.numCallpaths();
    for (ident_t id = 0; id < count; ++id) {
        Callpath* callpath = defs.get_cnode(id);

        // Special case: "UNKNOWN" callpath found in EPIK traces
        const Region& region = callpath->getRegion();
        if (("UNKNOWN" == region.getDisplayName().getString())
            && ("EPIK" == region.getFilename().getString())
            && ("EPIK" == region.getDescription().getString())) {
            // Create dummy entry
            data.cnodes.push_back(NULL);
            continue;
        }

        // Determine parent
        Callpath*   parent  = callpath->getParent();
        cube_cnode* cparent = NULL;
        if (parent) {
            cparent = data.cnodes[parent->getId()];
        }

        // Create node
        cube_cnode*     ccallpath;
        const Callsite& csite = callpath->getCallsite();
        if (Callsite::UNDEFINED == csite) {
            ccallpath = cube_def_cnode(data.cb,
                                       data.regions[region.getId()],
                                       cparent);
        } else {
            ccallpath = cube_def_cnode_cs(data.cb,
                                          data.regions[csite.getCallee().getId()],
                                          csite.getFilename().getCString(),
                                          csite.getLine(),
                                          cparent);
        }
        data.cnodes.push_back(ccallpath);
    }
}


/**
 *  @brief Generates the topology information.
 *
 *  This method defines the CUBE cartesian topology in the CUBE object passed
 *  via @a data based on the information available in the global definitions
 *  object.
 *
 *  @param  data  Report data structure
 **/
void
ReportWriterTask::gen_cartesians(ReportData& data) const
{
    const GlobalDefs& defs = *mSharedData.mDefinitions;

    // Create topologies
    uint32_t ncarts = defs.num_cartesians();
    for (ident_t cid = 0; cid < ncarts; ++cid) {
        Cartesian* cart  = defs.get_cartesian(cid);
        uint32_t   ndims = cart->num_dimensions();

        // Query topology information
        vector<uint32_t> dimv;
        vector<bool>     periodv;
        cart->get_dimensionv(dimv);
        cart->get_periodv(periodv);

        // Transform topology information to CUBE-conforming format
        long* dim    = new long[ndims];
        int*  period = new int[ndims];
        for (uint32_t index = 0; index < ndims; ++index) {
            dim[index]    = dimv[index];
            period[index] = periodv[index];
        }

        // Create cartesian
        cube_cartesian* ccart = cube_def_cart(data.cb, ndims, dim, period);

        // Release memory
        delete[] dim;
        delete[] period;

        // Define the coordinates
        long*    coord = new long[ndims];
        uint32_t nlocs = defs.numLocations();
        for (uint32_t index = 0; index < nlocs; ++index) {
            const Location& location = defs.getLocationByIndex(index);

            try {
                // Query coordinate information
                vector<uint32_t> coordv;
                cart->get_coords(location, coordv);

                // Transform coordinate to CUBE-conforming format
                for (uint32_t index = 0; index < ndims; ++index) {
                    coord[index] = coordv[index];
                }

                // Define coordinate
                cube_def_coords(data.cb, ccart, data.locs[location.getId()], coord);
            }
            catch (const Error&) {
            }
        }

        // Release memory
        delete[] coord;
    }
}
