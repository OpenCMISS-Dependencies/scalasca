/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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
#include <pearl/MpiGroup.h>

#include <iostream>

#include <pearl/String.h>

#include "pearl_iomanip.h"

using namespace std;
using namespace pearl;


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiGroup.cpp
 *  @ingroup PEARL_mpi
 *  @brief   Implementation of the class MpiGroup.
 *
 *  This file provides the implementation of the class MpiGroup.
 **/
/*-------------------------------------------------------------------------*/


//--- Constructors & destructor ---------------------------------------------

MpiGroup::MpiGroup(const IdType         id,
                   const String&        name,
                   const RankContainer& ranks,
                   const bool           isSelf,
                   const bool           isWorld)
    : ProcessGroup(id, name, ranks, isSelf, isWorld),
      mGroup(MPI_GROUP_NULL)
{
    // MPI group handle will be lazily created on first access by the
    // getGroup() method.
}


MpiGroup::~MpiGroup()
{
    // Free MPI resources
    if ((MPI_GROUP_NULL != mGroup)
        && (MPI_GROUP_EMPTY != mGroup)) {
        MPI_Group_free(&mGroup);
    }
}


//--- Get MPI handles -------------------------------------------------------

MPI_Group
MpiGroup::getGroup()
{
    // Lazy generation of MPI group handle at first access
    if (MPI_GROUP_NULL == mGroup) {
        mGroup = createGroup();
    }
    return mGroup;
}


//--- Private methods -------------------------------------------------------

/// @brief Stream output helper function.
///
/// Prints the instance data to the given @a stream in a human-readable form.
/// It therefore is the low-level implementation of operator<<().
///
/// @param  stream  Output stream
///
/// @return Output stream
///
ostream&
MpiGroup::print(ostream& stream) const
{
    // Adjust indentation
    int indent = getIndent(stream);
    setIndent(stream, indent + 9);

    // Print data
    stream << "MpiGroup {" << iendl(indent)
           << "  id   = " << getId() << iendl(indent)
           << "  name = " << getName() << iendl(indent)
           << "}";

    // Reset indentation
    return setIndent(stream, indent);
}


/// @brief Create corresponding MPI group.
///
/// Creates an MPI group consisting of the processes corresponding to the
/// global process identifiers stored in this MpiGroup object and returns
/// its handle.
///
/// @return MPI group handle
///
MPI_Group
MpiGroup::createGroup() const
{
    MPI_Group result;

    if (0 == numRanks()) {
        // Special case 1: GROUP_EMPTY
        result = MPI_GROUP_EMPTY;
    } else if (isSelf()) {
        // Special case 2: GROUP_SELF
        MPI_Comm_group(MPI_COMM_SELF, &result);
    } else if (isWorld()) {
        // Special case 3: GROUP_WORLD
        MPI_Comm_group(MPI_COMM_WORLD, &result);
    } else {
        // Create ranks array
        uint32_t size  = numRanks();
        int*     ranks = new int[size];
        for (uint32_t rank = 0; rank < size; ++rank) {
            ranks[rank] = getGlobalRank(rank);
        }

        // Create group based on world group
        MPI_Group world_group;
        MPI_Comm_group(MPI_COMM_WORLD, &world_group);
        MPI_Group_incl(world_group, size, ranks, &result);
        MPI_Group_free(&world_group);

        // Destroy ranks array
        delete[] ranks;
    }

    return result;
}
