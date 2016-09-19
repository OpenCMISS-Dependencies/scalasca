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


#include <config.h>
#include "Predicates.h"

#include <pearl/Region.h>

using namespace pearl;


/**
 *  Returns TRUE if the MPI_COLLECTIVE_END event is on the
 *  root process of a collective operation.
 **/
bool isOnRoot(const Event& event, int rank)
{
  return (event->getRoot() == static_cast<uint32_t>(rank));
}


bool isLogicalSend(const Event& event, int rank)
{
    // Obviously, (non-)blocking SEND events are also logical sends
    if (event->isOfType(GROUP_SEND))
        return true;

    event_t type = event->getType();
    if (MPI_COLLECTIVE_BEGIN == type) {
        const Region & region = event.enterptr()->getRegion();

        if (is_mpi_barrier(region) || is_mpi_n2n(region) || is_mpi_scan(region))
            return true;

        const Event& end = event.endptr();
        if (is_mpi_12n(region) && isOnRoot(end, rank))
            return true;

        if (is_mpi_n21(region) && !isOnRoot(end, rank))
            return true;

        return false;
    }

#if defined(_OPENMP)
    if (THREAD_FORK == type)
        return true;

    if (THREAD_TEAM_END == type)
        return true;

    if (event->isOfType(GROUP_ENTER)) {
        const Region & region = event->getRegion();

        return is_omp_barrier(region);
    }

#endif   // _OPENMP

    return false;
}
