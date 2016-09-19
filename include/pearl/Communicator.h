/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Communicator.
 *
 *  This header file provides the declaration of the class Communicator and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_COMMUNICATOR_H
#define PEARL_COMMUNICATOR_H


#include <iosfwd>

#include <stdint.h>

#include <pearl/Paradigm.h>


namespace pearl
{
//--- Forward declarations --------------------------------------------------

class ProcessGroup;
class String;


/*-------------------------------------------------------------------------*/
/**
 *  @class   Communicator
 *  @ingroup PEARL_base
 *  @brief   Representation of a communicator definition.
 *
 *  Instances of the Communicator class provide information about MPI
 *  communicators. Communicators are characterized by a name and a parent
 *  communicator as well as an associated group of processes. The numerical
 *  identifiers of the individual communicators are globally defined and
 *  continuously enumerated, i.e., the ID is element of
 *  [0,@#communicators-1].
 **/
/*-------------------------------------------------------------------------*/

class Communicator
{
    public:
        //--- Type definitions -----------------------------

        /// Communicator definition identifier type.
        typedef uint32_t IdType;


        //--- Static class data ----------------------------

        /// Numerical identifier used for undefined communicator definitions.
        static const IdType NO_ID = (~static_cast<IdType>(0));

        /// Static instance of class Communicator representing undefined
        /// communicator entries in other definitions.
        static const Communicator UNDEFINED;


        //--- Public methods -------------------------------

        /// @name Constructors & destructor
        /// @{


        /// @brief Constructor.
        ///
        /// Creates a new communicator definition instance and initializes
        /// it with the given values.
        ///
        /// @param  id        Global communicator identifier
        /// @param  name      Communicator name
        /// @param  group     Associated process group object
        /// @param  paradigm  Communicator paradigm
        /// @param  parent    Parent communicator (NULL for root communicators
        ///                   or when this information is not available)
        ///
        Communicator(IdType              id,
                     const String&       name,
                     const ProcessGroup& group,
                     Paradigm            paradigm,
                     Communicator*       parent);

        /// @brief Destructor.
        ///
        /// Releases all allocated resources and destroys the instance.
        ///
        virtual
        ~Communicator();

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get global identifier.
        ///
        /// Returns the global identifier of the communicator definition.
        ///
        /// @return Global identifier
        ///
        IdType
        getId() const;

        /// @brief Get communicator name.
        ///
        /// Returns the string definition storing the communicators's name. If
        /// this information is not available, String::UNDEFINED is returned.
        ///
        /// @return Communicator name
        ///
        const String&
        getName() const;

        /// @brief Get associated process group.
        ///
        /// Returns the ProcessGroup object associated to this communicator.
        ///
        /// @return %Process group object
        ///
        const ProcessGroup&
        getGroup() const;

        /// @brief Get parent communicator.
        ///
        /// Returns a pointer to the parent communicator if applicable.
        /// Returns NULL for root communicators or when this information is
        /// not available.
        ///
        /// @return Parent communicator
        ///
        Communicator*
        getParent() const;

        /// @brief Get communicator paradigm.
        ///
        /// Returns the paradigm of the communicator. The paradigm can be
        /// used to distiguish communicators of different message passing
        /// libraries, such as MPI, ARMCI, SHMEM, etc..
        ///
        /// @return Paradigm
        Paradigm
        getParadigm() const;

        /// @}


    private:
        //--- Data members ---------------------------------

        /// Global identifier
        IdType mIdentifier;

        /// Communicator name
        const String& mName;

        /// Associated group of processes
        const ProcessGroup& mGroup;

        /// Associated communication paradigm
        Paradigm mParadigm;

        /// Parent communicator
        Communicator* mParent;


        //--- Private methods & friends --------------------

        virtual std::ostream&
        print(std::ostream& stream) const;

        friend std::ostream&
        operator<<(std::ostream&       stream,
                   const Communicator& item);
};


//--- Related functions -----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates Communicator
///
/// Prints the contents of the given communicator definition object @a item
/// to an output @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    Communicator definition object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream&       stream,
           const Communicator& item);

/// @}
/// @name Comparison operators
/// @{

/// @brief   Equality operator.
/// @relates Communicator
///
/// Compares two communicator definition objects @a lhs and @a rhs for
/// equality. Two communicators are considered equal if their identifiers,
/// names, associated process groups, and parants are identical.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the communicator definitions are equal, false otherwise.
///
bool
operator==(const Communicator& lhs,
           const Communicator& rhs);

/// @brief   Inequality operator.
/// @relates Communicator
///
/// Compares two communicator definition objects @a lhs and @a rhs for
/// inequality. Two communicators are considered unequal if either their
/// IDs, names, associated process groups, or parents differ.
///
/// @param  lhs  Left-hand operand
/// @param  rhs  Right-hand operand
///
/// @return True if the communicator definitions differ, false otherwise.
///
bool
operator!=(const Communicator& lhs,
           const Communicator& rhs);

/// @}
}   // namespace pearl


#endif   // !PEARL_COMMUNICATOR_H
