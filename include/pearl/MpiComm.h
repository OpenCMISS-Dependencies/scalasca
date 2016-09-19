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


#ifndef PEARL_MPICOMM_H
#define PEARL_MPICOMM_H


#include <mpi.h>

#include "Communicator.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiComm.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiComm.
 *
 *  This header file provides the declaration of the class MpiComm.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
//--- Forward declarations --------------------------------------------------

class MpiGroup;


/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiComm
 *  @ingroup PEARL_mpi
 *  @brief   Stores information related to an MPI communicator.
 *
 *  Instances of the MpiComm class provide information about MPI
 *  communicators. In addition to the functionality of the base class
 *  Communicator, they also provide a "real" MPI communicator handle
 *  resembling the one used in the target application.
 **/
/*-------------------------------------------------------------------------*/

class MpiComm
    : public Communicator
{
    public:
        //--- Public methods -------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new MPI communicator definition instance and initializes
        /// it with the given values.
        ///
        /// @param  id      Global communicator identifier
        /// @param  name    Communicator name
        /// @param  group   Associated process group object
        /// @param  parent  Parent communicator (NULL for root communicators
        ///                 or when this information is not available)
        ///
        MpiComm(IdType          id,
                const String&   name,
                const MpiGroup& group,
                MpiComm*        parent);

        /// @brief Destructor.
        ///
        /// Releases all allocated resources and destroys the instance.
        ///
        virtual
        ~MpiComm();

        /// @}
        /// @name Get MPI handles
        /// @{

        /// @brief Get MPI communicator handle
        ///
        /// Returns the MPI communicator handle associated with this
        /// communicator object.
        ///
        /// @return MPI communicator handle
        ///
        MPI_Comm
        getComm() const;

        /// @}
        /// @name Communicator duplication
        /// @{

        MpiComm*
        duplicate() const;

        /// @}


    protected:
        //--- Protected methods ----------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Copy constructor.
        ///
        /// Copies the given instance and duplicates the associated MPI
        /// communicator handle using MPI_Comm_dup. It is 'protected' to
        /// prevent accidental invocation; users need to call
        /// MpiComm::duplicate() explicitly to create a communicator copy.
        ///
        /// @param  comm  MPI communicator definition to duplicate
        ///
        MpiComm(const MpiComm& comm);

        /// @}


    private:
        //--- Data members ---------------------------------

        /// Corresponding MPI communicator
        MPI_Comm mCommunicator;


        //--- Private methods & friends --------------------

        virtual std::ostream&
        print(std::ostream& stream) const;
};
}   // namespace pearl


#endif   // !PEARL_MPICOMM_H
