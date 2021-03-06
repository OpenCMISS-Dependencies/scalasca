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
 *  @brief   Declaration of the class EpikArchive.
 *
 *  This header file provides the declaration of the class EpikArchive and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_EPIKARCHIVE_H
#define PEARL_EPIKARCHIVE_H


#include <map>
#include <vector>

#include <pearl/Location.h>
#include <pearl/TraceArchive.h>


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @ingroup PEARL_base
 *  @brief   Representation of an EPIK trace experiment archive.
 *
 *  The %EpikArchive class provides a representation for legacy trace
 *  experiment archives in EPILOG format generated by the EPIK measurement
 *  system of the Scalasca v1 release series. It implements the common
 *  interface defined by the %TraceArchive class, hiding the details of using
 *  the low-level EPILOG library. Instances of %EpikArchive have to be created
 *  using the TraceArchive::open() factory method.
 **/
/*-------------------------------------------------------------------------*/

class EpikArchive
    : public TraceArchive
{
    public:
        //--- Public member functions ----------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new EPIK trace experiment archive handle and initializes
        /// both the anchor and archive directory name with the name of the
        /// provided anchor directory @a archiveDir.
        ///
        /// @param archiveDir
        ///     Name of the trace experiment archive directory
        ///
        EpikArchive(const std::string& archiveDir);

        /// @}
        /// @name Mapping location IDs
        /// @{

        /// @brief Add identifier mapping.
        ///
        /// Adds a new EPIK-to-internal location identifier mapping.
        ///
        /// @param epkLocationId
        ///     EPIK location identifier
        /// @param intLocationId
        ///     Internal location identifier
        ///
        void
        addIdMapping(uint32_t         epkLocationId,
                     Location::IdType intLocationId);

        /// @brief Map location identifier.
        ///
        /// Maps an EPIK location identifier to the corresponding internal
        /// location identifier.
        ///
        /// @param epkLocationId
        ///     EPIK location identifier
        /// @returns
        ///     Internal location identifier
        ///
        Location::IdType
        mapLocationId(uint32_t epkLocationId) const;

        /// @}
        /// @name Mapping file offset handling
        /// @{

        /// @brief Set mapping file offset.
        ///
        /// Stores the file offset for the mapping data of the given @a rank
        /// as provided by the 'epik.map' file.
        ///
        /// @param rank
        ///     MPI rank
        /// @param offset
        ///     File offset
        ///
        void
        setOffset(uint32_t rank,
                  uint32_t offset);

        /// @brief Get mapping file offset.
        ///
        /// Returns the file offset for the mapping data of the given @a rank
        /// as provided by the 'epik.map' file.
        ///
        /// @param rank
        ///     MPI rank
        /// @returns
        ///     File offset
        ///
        uint32_t
        getOffset(uint32_t rank) const;

        /// @}


    private:
        //--- Type definitions -----------------------------

        /// Container type for location ID mapping
        typedef std::map<uint32_t, Location::IdType> LocationMap;

        /// Container type for mapping file offsets
        typedef std::vector<uint32_t> OffsetContainer;


        //--- Data members ---------------------------------

        /// %Location ID mapping: EPIK location ID |-@> internal ID
        LocationMap mLocationMap;

        /// Mapping file offset values: rank |-@> file offset
        OffsetContainer mOffsets;


        //--- Private member functions & friends -----------

        virtual void
        openArchive();
        virtual void
        readDefinitions(GlobalDefs* defs);
        virtual void
        openContainer(const LocationGroup& locGroup);
        virtual void
        closeContainer();
        virtual LocalIdMaps*
        readIdMaps(const Location& location);
        virtual void
        readTrace(const GlobalDefs& defs,
                  const Location&   location,
                  LocalIdMaps*      idMaps,
                  LocalTrace*       trace);
};
}   // namespace detail
}   // namespace pearl


#endif   // !PEARL_EPIKARCHIVE_H
