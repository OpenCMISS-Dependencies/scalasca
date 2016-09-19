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
 *  @ingroup PEARL_mpi
 *  @brief   Implementation of the class MpiComm.
 *
 *  This file provides the implementation of the class MpiComm.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include <pearl/MpiComm.h>

#include <iostream>

#include <pearl/MpiGroup.h>
#include <pearl/String.h>

#include "pearl_iomanip.h"

using namespace std;
using namespace pearl;


//--- Constructors & destructor ---------------------------------------------

MpiComm::MpiComm(const IdType    id,
                 const String&   name,
                 const MpiGroup& group,
                 MpiComm* const  parent)
    : Communicator(id, name, group, Paradigm::MPI, parent),
      mCommunicator(MPI_COMM_NULL)
{
    MPI_Group group_handle;

    // Create communicator
    group_handle = group.createGroup();
    MPI_Comm_create(MPI_COMM_WORLD, group_handle, &mCommunicator);
    MPI_Group_free(&group_handle);
}


MpiComm::MpiComm(const MpiComm& comm)
    : Communicator(comm),
      mCommunicator(MPI_COMM_NULL)
{
    MPI_Comm_dup(comm.mCommunicator, &mCommunicator);
}


MpiComm::~MpiComm()
{
    // Free resources
    if (MPI_COMM_NULL != mCommunicator) {
        MPI_Comm_free(&mCommunicator);
    }
}


//--- Get MPI handles -------------------------------------------------------

MPI_Comm
MpiComm::getComm() const
{
    return mCommunicator;
}


//--- Communicator duplication ----------------------------------------------

MpiComm*
MpiComm::duplicate() const
{
    return new MpiComm(*this);
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
MpiComm::print(ostream& stream) const
{
    // Adjust indentation
    int indent = getIndent(stream);
    setIndent(stream, indent + 9);

    // Print data
    stream << "MpiComm {" << iendl(indent)
           << "  id   = " << getId() << iendl(indent)
           << "  name = " << getName() << iendl(indent)
           << "}";

    // Reset indentation
    return setIndent(stream, indent);
}
