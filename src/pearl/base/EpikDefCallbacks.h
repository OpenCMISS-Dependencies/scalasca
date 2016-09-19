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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declarations of EPIK callback functions for definition records.
 *
 *  This header file provides the declarations of EPIK callback functions
 *  and related data types for handling global definition records.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_EPIKDEFCALLBACKS_H
#define PEARL_EPIKDEFCALLBACKS_H


#include <map>
#include <string>
#include <vector>

#include <pearl/pearl_types.h>


namespace pearl
{
//--- Forward declarations --------------------------------------------------

class GlobalDefs;


namespace detail
{
//--- Forward declarations --------------------------------------------------

class EpikArchive;


/*-------------------------------------------------------------------------*/
/**
 *  @struct  EpikDefCbData
 *  @ingroup PEARL_base
 *  @brief   EPIK definition callback data
 *
 *  The EpikDefCbData struct combines all data elements which are necessary
 *  to process the global definition records of an EPIK trace archive.
 **/
/*-------------------------------------------------------------------------*/

struct EpikDefCbData
{
    //--- Public methods -------------------------------

    /// @name Constructors & destructor
    /// @{

    EpikDefCbData(EpikArchive& archive,
                  GlobalDefs&  defs);

    /// @}
    /// @name Mapping functions
    /// @{

    ident_t
    get_file(ident_t id) const;

    /// @}


    //--- Data members ---------------------------------

    /// EPIK archive abstraction
    EpikArchive& mArchive;

    /// %GlobalDefs object storing the definitions read
    GlobalDefs& mDefinitions;

    /// Number of expected string definition continuation records
    uint8_t mStringContCount;

    /// Identifier of the string definition to be continued
    ident_t mStringContId;

    /// Temporary storage to concatenate continued string definition
    std::string mStringContData;

    /// Size of MPI_COMM_WORLD (always 1 in case of serial/OpenMP experiments)
    uint32_t mWorldSize;

    /// Mapping table file ID |-@> string ID
    std::vector<ident_t> mFileMap;

    /// Mapping table machine ID |-@> system node ID
    std::map<ident_t, ident_t> mMachineMap;

    /// Mapping table node ID |-@> system node ID
    std::map<ident_t, ident_t> mNodeMap;

    /// Mapping table process ID |-@> string ID
    std::map<ident_t, ident_t> mProcessMap;

    /// Mapping table (process ID, thread ID) |-@> string ID
    std::map<std::pair<ident_t, ident_t>, ident_t> mThreadMap;

    /// %Error message of exceptions thrown (and caught) within callbacks
    std::string mErrorMessage;
};
}   // namespace detail
}   // namespace pearl


#endif   // !PEARL_EPIKDEFCALLBACKS_H
