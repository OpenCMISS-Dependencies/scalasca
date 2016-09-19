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


#ifndef PEARL_PROCESSGROUP_H
#define PEARL_PROCESSGROUP_H


#include <iosfwd>
#include <vector>

#include <stdint.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    ProcessGroup.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class ProcessGroup.
 *
 *  This header file provides the declaration of the class ProcessGroup and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
//--- Forward declarations --------------------------------------------------

class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   ProcessGroup
 *  @ingroup PEARL_base
 *  @brief   Defines a group of processes by means of global process
 *           identifiers.
 *
 *  The class ProcessGroup provides a way to group multiple processes (e.g.,
 *  MPI ranks) and to identify such groups using an unique identifier. This
 *  is accomplished by storing an ordered set of global process identifiers
 *  (i.e., global ranks). Due to the ordering, each process in the group is
 *  implicitly assigned a local identifier (local rank) in the range
 *  [0,@#members-1] with respect to this group. ProcessGroup objects are used
 *  to define MPI groups (see MpiGroup) and are referenced by MPI
 *  communicator objects (see Comm and MpiComm). The numerical identifiers of
 *  the individual groups are globally defined and continuously enumerated,
 *  i.e., the ID is element of [0,@#process_groups-1].
 **/
/*-------------------------------------------------------------------------*/

class ProcessGroup
{
    public:
        //--- Type definitions -----------------------------

        /// %ProcessGroup definition identifier type.
        typedef uint32_t IdType;

        /// Container type for process identifiers (i.e., MPI ranks).
        typedef std::vector<int> RankContainer;


        //--- Static class data ----------------------------

        /// Numerical identifier used for undefined process group definitions.
        static const IdType NO_ID = (~static_cast<IdType>(0));

        /// Static instance of class ProcessGroup representing undefined
        /// process group entries in other definitions.
        static const ProcessGroup UNDEFINED;



        //--- Public methods -------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a process group definition instance and initializes it
        /// with the given values. The flags @a isSelf and @a isWorld are
        /// mutually exclusive, except for single-process experiments.
        ///
        /// @param id      Global process group identifier
        /// @param name    %Process group name
        /// @param ranks   Ordered list of global process identifiers
        ///                defining the group
        /// @param isSelf  Indicates whether this group refers to 'SELF'
        /// @param isWorld Indicates whether this group refers to 'WORLD'
        ///
        ProcessGroup(IdType               id,
                     const String&        name,
                     const RankContainer& ranks,
                     bool                 isSelf,
                     bool                 isWorld);

        /// @brief Destructor.
        ///
        /// Releases all allocated resources and destroys the instance.
        ///
        virtual
        ~ProcessGroup();

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the process group definition.
        ///
        /// @return Global identifier
        ///
        IdType
        getId() const;

        /// @brief Get process group name.
        ///
        /// Returns the string definition storing the process group's name. If
        /// this information is not available, String::UNDEFINED is returned.
        ///
        /// @return %Process group name
        ///
        const String&
        getName() const;

        /// @brief Get number of processes.
        ///
        /// Returns the number of associated processes in this group.
        ///
        /// @return  Number of processes
        ///
        uint32_t
        numRanks() const;

        /// @brief Convert global to local rank.
        ///
        /// Returns the local rank with respect to this group of the process
        /// with the given global process identifier @a globalRank.
        ///
        /// @param globalRank Global process identifier
        ///
        /// @return Local rank within this group
        ///
        /// @exception RuntimeError if the process with @a globalRank is not
        ///                         part of this group ("Unknown identifier")
        ///
        int
        getLocalRank(int globalRank) const;

        /// @brief Convert local to global rank.
        ///
        /// Returns the global process identifier of the process with the
        /// given @a localRank with respect to this group.
        ///
        /// @param localRank Local rank within this group
        ///
        /// @return Global process identifier
        ///
        /// @exception RuntimeError if an invalid @a localRank is given ("Rank
        ///                         out of range")
        ///
        int
        getGlobalRank(int localRank) const;

        /// Returns whether this group is a 'SELF' group.
        ///
        /// @return Flag indicating 'SELF' status.
        ///
        bool
        isSelf() const;

        /// Returns whether this group is a 'WORLD' group.
        ///
        /// @return Flag indicating 'WORLD' status.
        ///
        bool
        isWorld() const;

        /// @}


    private:
        //--- Data members ---------------------------------

        /// Global identifier
        IdType mIdentifier;

        /// %Process group name
        const String& mName;

        /// Ordered list of global process identifiers (i.e., MPI ranks)
        RankContainer mRanks;

        /// Group flags (SELF, WORLD, etc.)
        unsigned char mFlags;


        //--- Private methods & friends --------------------

        virtual std::ostream&
        print(std::ostream& stream) const;

        friend std::ostream&
        operator<<(std::ostream&       stream,
                   const ProcessGroup& item);
};


//--- Related functions -----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates ProcessGroup
///
/// Prints the contents of the given process group definition object @a item
/// to an output @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    %Process group definition object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream&       stream,
           const ProcessGroup& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates ProcessGroup
///
/// Compares two process group definition objects @a lhs and @a rhs for
/// equality. Two process groups are considered equal if their identifiers,
/// names, and the size of the rank lists are identical.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the process group definitions are equal, false otherwise.
///
bool
operator==(const ProcessGroup& lhs,
           const ProcessGroup& rhs);

/// @brief   Inequality operator.
/// @relates ProcessGroup
///
/// Compares two process group definition objects @a lhs and @a rhs for
/// inequality. Two process groups are considered unequal if either their
/// IDs, names, or the size of the rank lists differ.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the process group definitions differ, false otherwise.
///
bool
operator!=(const ProcessGroup& lhs,
           const ProcessGroup& rhs);

/// @}
}   // namespace pearl


#endif   // !PEARL_PROCESSGROUP_H
