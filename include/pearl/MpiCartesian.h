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


#ifndef PEARL_MPICARTESIAN_H
#define PEARL_MPICARTESIAN_H


#include "Cartesian.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiCartesian.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiCartesian.
 *
 *  This header file provides the declaration of the class MpiCartesian.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class MpiComm;


/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiCartesian
 *  @ingroup PEARL_mpi
 *  @brief   Stores information related to a virtual cartesian MPI topology.
 *
 *  @note The Communicator object associated with a MPI topology is only
 *        referenced and not owned by the corresponding MpiCartesian object.
 *        Therefore, it will not be deleted if the MpiCartesian object is
 *        released.
 **/
/*-------------------------------------------------------------------------*/

class MpiCartesian : public Cartesian
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiCartesian(uint32_t           id,
                 const cart_dims&   num_locations,
                 const cart_period& is_periodic,
                 MpiComm*           communicator);

    /// @}
    /// @name Get MPI cartesian topology information
    /// @{

    MpiComm* get_comm() const;

    /// @}


  private:
    /// Associated MPI communicator of the topology
    MpiComm* m_communicator;
};


}   // namespace pearl


#endif   // !PEARL_MPICARTESIAN_H
