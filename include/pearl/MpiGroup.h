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


#ifndef PEARL_MPIGROUP_H
#define PEARL_MPIGROUP_H


#include <mpi.h>

#include "ProcessGroup.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file    MpiGroup.h
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiGroup.
 *
 *  This header file provides the declaration of the class MpiGroup.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiGroup
 *  @ingroup PEARL_mpi
 *  @brief   Stores information related to an MPI group.
 *
 *  Instances of the MpiGroup class provide information about MPI groups. In
 *  addition to the functionality of the base class ProcessGroup, they also
 *  provide a "real" MPI group handle resembling the one used in the target
 *  application.
 **/
/*-------------------------------------------------------------------------*/

class MpiGroup
    : public ProcessGroup
{
    public:
        //--- Public methods -------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a MPI group definition instance and initializes it with
        /// the given values. The flags @a isSelf and @a isWorld are
        /// mutually exclusive, except for single-process experiments.
        ///
        /// @param id      Global MPI group identifier
        /// @param name    MPI group name
        /// @param ranks   Ordered list of global process identifiers (i.e.,
        ///                ranks) defining the group
        /// @param isSelf  Indicates whether this group refers to 'SELF'
        /// @param isWorld Indicates whether this group refers to 'WORLD'
        ///
        MpiGroup(IdType               id,
                 const String&        name,
                 const RankContainer& ranks,
                 bool                 isSelf,
                 bool                 isWorld);


        /// @brief Destructor.
        ///
        /// Releases all allocated resources and destroys the instance.
        ///
        virtual
        ~MpiGroup();

        /// @}
        /// @name Get MPI handles
        /// @{

        /// @brief Get MPI group handle.
        ///
        /// Returns the MPI group handle associated with this group object.
        /// The MPI group is lazily created on first access and then cached
        /// until the instance is destructed.
        ///
        /// @return MPI group handle
        ///
        MPI_Group
        getGroup();

        /// @}


    private:
        //--- Data members ---------------------------------

        /// Corresponding MPI group handle
        MPI_Group mGroup;


        //--- Private methods & friends --------------------

        virtual std::ostream&
        print(std::ostream& stream) const;
        MPI_Group
        createGroup() const;

        friend class MpiComm;
};
}   // namespace pearl


#endif   // !PEARL_MPIGROUP_H
