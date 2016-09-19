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


#ifndef PEARL_TOPOLOGY_H
#define PEARL_TOPOLOGY_H


#include <stdint.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    Topology.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Topology.
 *
 *  This header file provides the declaration of the class Topology.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 *  @class   Topology
 *  @ingroup PEARL_base
 *  @brief   Base class for virtual topologies.
 *
 *  The class Topology serves as a base class for all classes related to
 *  virtual topologies.
 *
 *  The numerical identifiers of the individual topologies are globally
 *  defined and continuously enumerated, i.e., the ID is element of
 *  [0,@#topologies-1].
 **/
/*-------------------------------------------------------------------------*/

class Topology
{
  public:
    /// @name Constructors & destructor
    /// @{

    virtual ~Topology();

    /// @}
    /// @name Get topology information
    /// @{

    uint32_t get_id() const;

    /// @}


  protected:
    /// @name Constructors & destructor
    /// @{

    Topology(uint32_t id);

    /// @}


  private:
    /// Global topology identifier
    uint32_t m_id;
};


}   // namespace pearl


#endif   // !PEARL_TOPOLOGY_H
