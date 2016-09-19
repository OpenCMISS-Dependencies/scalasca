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
#include "MpiDefsFactory.h"

#include <sstream>

#include <mpi.h>

#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiCartesian.h>
#include <pearl/MpiComm.h>
#include <pearl/MpiGroup.h>
#include <pearl/MpiWindow.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class MpiDefsFactory
//
//---------------------------------------------------------------------------

//--- MPI-specific factory methods ------------------------------------------

/// @todo Fix to handle non-process location groups
void MpiDefsFactory::createMpiGroup(GlobalDefs&    defs,
                                    uint32_t       id,
                                    uint32_t       nameId,
                                    process_group& ranks,
                                    bool           isSelf,
                                    bool           isWorld) const
{
  // Sanity check
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  // FIXME: Handle non-process location groups
  if (static_cast<uint32_t>(world_size) < ranks.size()) {
    ostringstream num;
    num << defs.numLocationGroups();

    throw FatalError("MpiDefsFactory::createMpiGroup() -- "
                     "Experiment requires at least " + num.str() + " MPI ranks.");
  }

  /* handling of 'self' group */
  if (isSelf) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    ranks.push_back(rank);
  }

  // Create & store new MPI group object
  defs.addProcessGroup(new MpiGroup(id,
                                    defs.getString(nameId),
                                    ranks,
                                    isSelf,
                                    isWorld));
}


void MpiDefsFactory::createMpiComm(GlobalDefs& defs,
                                   uint32_t    id,
                                   uint32_t    nameId,
                                   uint32_t    groupId,
                                   uint32_t    parentId) const
{
  // Determine MPI process group
  const MpiGroup& group = dynamic_cast<const MpiGroup&>(defs.getProcessGroup(groupId));

  MpiComm*      parent = NULL;
  Communicator* comm   = defs.get_comm(parentId);
  if (NULL != comm)
    parent = dynamic_cast<MpiComm*>(comm);

  // Create & store new MPI communicator object
  defs.addCommunicator(new MpiComm(id,
                                   defs.getString(nameId),
                                   group,
                                   parent));
}


void MpiDefsFactory::createMpiWindow(GlobalDefs& defs,
                                     ident_t     id,
                                     ident_t     comm_id) const
{
  // Determine communicator
  MpiComm* comm = dynamic_cast<MpiComm*>(defs.get_comm(comm_id));
  if (!comm)
    throw RuntimeError("MpiDefsFactory::createMpiWindow() -- "
                       "Invalid MPI communicator ID.");

  // Create & store new MPI RMA window
  defs.add_window(new MpiWindow(id, comm));
}


void MpiDefsFactory::createMpiCartesian(GlobalDefs&        defs,
                                        ident_t            id,
                                        const cart_dims&   dimensions,
                                        const cart_period& periodic,
                                        ident_t            comm_id) const
{
  // Determine communicator
  MpiComm* comm = dynamic_cast<MpiComm*>(defs.get_comm(comm_id));
  if (!comm)
    throw RuntimeError("MpiDefsFactory::createMpiCartesian() -- "
                       "Invalid MPI communicator ID.");

  // Create & store new MPI cartesian topology object
  defs.add_cartesian(new MpiCartesian(id, dimensions, periodic, comm));
}
