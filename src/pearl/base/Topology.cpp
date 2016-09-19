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
#include <pearl/Topology.h>

using namespace pearl;


//---------------------------------------------------------------------------
//
//  class Topology
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

Topology::~Topology()
{
}


//--- Get topology information ----------------------------------------------

uint32_t Topology::get_id() const
{
  return m_id;
}


//--- Constructors & destructor (protected) ---------------------------------

/**
 *  Creates a new instance and initializes its identifier with the given
 *  value @a id.
 *
 *  @param id Topology identifier
 **/
Topology::Topology(uint32_t id)
  : m_id(id)
{
}
