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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class EpikArchive.
 *
 *  This file provides the implementation of the class EpikArchive and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "EpikArchive.h"

#include <cstdlib>
#include <memory>

#include <sys/stat.h>

#include <pearl/Error.h>
#include <pearl/LocalTrace.h>

#include "elg_error.h"
#include "elg_readcb.h"
#include "epk_archive.h"

#include "EpikDefCallbacks.h"
#include "EpikEvtCallbacks.h"
#include "EpikMapCallbacks.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


//--- Constructors & destructor ---------------------------------------------

EpikArchive::EpikArchive(const string& archiveDir)
    : TraceArchive(archiveDir, archiveDir)
{
    // Sanity checks
    const char* name = archiveDir.c_str();
    if (!epk_archive_set_name(name)) {
        throw RuntimeError("Experiment archive \"" + archiveDir
                           + "\" is not an EPIK archive.");
    }
    if (!epk_archive_exists(name)) {
        throw RuntimeError("Experiment archive \"" + archiveDir
                           + "\"does not exist.");
    }
}


//--- Mapping location IDs --------------------------------------------------

void
EpikArchive::addIdMapping(uint32_t         epkLocationId,
                          Location::IdType intLocationId)
{
    LocationMap::iterator it = mLocationMap.find(epkLocationId);
    if (mLocationMap.end() != it) {
        throw FatalError("EpikArchive::addIdMapping(uint32_t,Location::IdType)"
                         " -- mapping already defined!");
    }
    mLocationMap.insert(it, LocationMap::value_type(epkLocationId,
                                                    intLocationId));
}


Location::IdType
EpikArchive::mapLocationId(uint32_t epkLocationId) const
{
    LocationMap::const_iterator it = mLocationMap.find(epkLocationId);
    if (mLocationMap.end() == it) {
        throw FatalError("EpikArchive::mapLocationId(uint32_t) -- unknown ID!");
    }
    return it->second;
}


//--- Mapping file offset handling ------------------------------------------

void
EpikArchive::setOffset(uint32_t rank,
                       uint32_t offset)
{
    if (rank >= mOffsets.size()) {
        mOffsets.resize(rank + 1);
    }
    mOffsets[rank] = offset;
}


uint32_t
EpikArchive::getOffset(uint32_t rank) const
{
    if (rank >= mOffsets.size()) {
        throw RuntimeError("EpikArchive::getOffset(uint32_t) -- Rank out of bounds");
    }
    return mOffsets[rank];
}


//--- Private member functions ----------------------------------------------

void
EpikArchive::openArchive()
{
}


void
EpikArchive::readDefinitions(GlobalDefs* defs)
{
    // Determine file name
    char*  tmp_str  = epk_archive_filename(EPK_TYPE_ESD, epk_archive_get_name());
    string filename = tmp_str;
    free(tmp_str);

    // Open definition file
    ElgRCB* file = elg_read_open(filename.c_str());
    if (!file) {
        throw RuntimeError("Cannot open EPILOG trace definition file \""
                           + filename + "\".");
    }

    // Read data
    EpikDefCbData data(*this, *defs);
    while (elg_read_next_def(file, &data)) {
        // Check for errors
        if (!data.mErrorMessage.empty()) {
            break;
        }
    }
    elg_read_close(file);

    // Check for errors
    if (!data.mErrorMessage.empty()) {
        throw Error(data.mErrorMessage);
    }
}


void
EpikArchive::openContainer(const LocationGroup& locGroup)
{
}


void
EpikArchive::closeContainer()
{
}


LocalIdMaps*
EpikArchive::readIdMaps(const Location& location)
{
    // Determine filename
    char*  tmp_str  = epk_archive_filename(EPK_TYPE_EMP, getAnchorName().c_str());
    string filename = tmp_str;
    free(tmp_str);

    // Create mapping data object
    auto_ptr<EpikMapCbData> mapData(new EpikMapCbData);

    // Check whether mapping file exists
    struct stat sbuf;
    if (stat(filename.c_str(), &sbuf) != 0) {
        elg_cntl_msg("No mapping file found -- Assuming already globalized trace");
        return mapData.release();
    }

    uint32_t rank   = location.getRank();
    uint32_t offset = 0;
    if (!mOffsets.empty()) {
        offset = getOffset(rank);
    }

    // Open mapping file
    ElgRCB* file = elg_read_open(filename.c_str());
    if (!file) {
        throw RuntimeError("Cannot open EPILOG mapping file \""
                           + filename + "\".");
    }

    // Read header
    elg_read_seek(file, offset);
    elg_read_next_def(file, mapData.get());
    if (mapData->mRank != rank) {
        elg_read_close(file);
        throw FatalError("Found unrelated mapping file section");
    }

    // Read data in section
    while (elg_read_next_def(file, mapData.get()) && mapData->mRank == rank) {
        // Check for errors
        if (!mapData->mErrorMessage.empty()) {
            break;
        }
    }
    elg_read_close(file);

    // Check for errors
    if (!mapData->mErrorMessage.empty()) {
        throw Error(mapData->mErrorMessage);
    }

    return mapData.release();
}


void
EpikArchive::readTrace(const GlobalDefs& defs,
                       const Location&   location,
                       LocalIdMaps*      idMaps,
                       LocalTrace*       trace)
{
    EpikMapCbData* mapData = static_cast<EpikMapCbData*>(idMaps);

    // Determine file name
    uint32_t rank     = location.getRank();
    char*    tmp_str  = epk_archive_rankname(EPK_TYPE_ELG,
                                             getAnchorName().c_str(), rank);
    string   filename = tmp_str;
    free(tmp_str);

    // Open local event trace file
    ElgRCB* file = elg_read_open(filename.c_str());
    if (!file) {
        throw RuntimeError("Cannot open EPILOG event trace file \""
                           + filename + "\".");
    }

    // Read trace data
    EpikEvtCbData data(*this, defs, *trace, *mapData, elg_read_version(file),
                       location.getId(), location.getThreadId());
    while (elg_read_next_event(file, &data)) {
        // Check for errors
        if (!data.mErrorMessage.empty()) {
            break;
        }
    }
    elg_read_close(file);

    // Check for errors
    if (!data.mErrorMessage.empty()) {
        throw Error(data.mErrorMessage);
    }

    // Consistency check
    if (!data.mCallstack.empty()) {
        throw  FatalError("Unbalanced ENTER/LEAVE events (Too many ENTERs).");
    }
}
