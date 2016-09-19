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
#include <pearl/MpiCartesian.h>

#include <cassert>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class MpiCartesian
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

/**
 *  Creates a new instance and initializes its identifier and communicator
 *  to the given values @a id and @a communicator, respectively.
 *
 *  @param id            Topology identifier
 *  @param num_locations Contains the number of locations in each dimension
 *  @param is_periodic   Contains boolean values defining whether the
 *                       corresponding dimension is periodic or not
 *  @param communicator  Associated MPI communicator
 **/
MpiCartesian::MpiCartesian(uint32_t           id,
                           const cart_dims&   num_locations,
                           const cart_period& is_periodic,
                           MpiComm*           communicator)
  : Cartesian(id, num_locations, is_periodic),
    m_communicator(communicator)
{
  assert(communicator);
}


//--- Get MPI topology information ------------------------------------------

/**
 *  Returns the MPI communicator associated to this topology.
 *
 *  @return Associated communicator
 **/
MpiComm* MpiCartesian::get_comm() const
{
  return m_communicator;
}
