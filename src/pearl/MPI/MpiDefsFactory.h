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


#ifndef PEARL_MPIDEFSFACTORY_H
#define PEARL_MPIDEFSFACTORY_H


#include "DefsFactory.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiDefsFactory.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiDefsFactory.
 *
 *  This header file provides the declaration of the class MpiDefsFactory.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiDefsFactory
 *  @ingroup PEARL_mpi
 *  @brief   Abstract factory class for MPI-specific global definition
 *           objects.
 **/
/*-------------------------------------------------------------------------*/

class MpiDefsFactory : public DefsFactory
{
  public:
    /// @name MPI-specific factory methods
    /// @{

    virtual void createMpiGroup    (GlobalDefs&        defs,
                                    uint32_t           id,
                                    uint32_t           nameId,
                                    process_group&     ranks,
                                    bool               isSelf,
                                    bool               isWorld) const;
    virtual void createMpiComm     (GlobalDefs&        defs,
                                    uint32_t           id,
                                    uint32_t           nameId,
                                    uint32_t           groupId,
                                    uint32_t           parentId) const;

    virtual void createMpiWindow   (GlobalDefs&        defs,
                                    ident_t            id,
                                    ident_t            comm_id) const;
    virtual void createMpiCartesian(GlobalDefs&        defs,
                                    ident_t            id,
                                    const cart_dims&   dimensions,
                                    const cart_period& periodic,
                                    ident_t            comm_id) const;

    /// @}
};


}   // namespace pearl


#endif   // !PEARL_MPIDEFSFACTORY_H
