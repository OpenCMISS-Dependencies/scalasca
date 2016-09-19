/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class ContextTree.
 *
 *  This header file provides the declaration of the class ContextTree and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_CONTEXTTREE_H
#define PEARL_CONTEXTTREE_H


#include <stdint.h>

#include <iosfwd>
#include <vector>

#include <pearl/CallingContext.h>


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   ContextTree
 *  @ingroup PEARL_base
 *  @brief   Container class for calling context trees.
 *
 *  Instances of the ContextTree class store the data of the global calling
 *  context tree.  Each context tree consists of a set of CallingContext
 *  objects encoding the actual parent-child relationships.
 **/
/*-------------------------------------------------------------------------*/

class ContextTree
{
    public:
        //--- Public methods -------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Destructor.
        ///
        /// Releases all allocated resources and destroys the instance.
        ///
        ~ContextTree();

        /// @}
        /// @name Query/modify the calling context tree information
        /// @{

        /// @brief Get number of calling contexts.
        ///
        /// Returns the total number of calling contexts currently stored in
        /// the context tree.
        ///
        /// @returns
        ///     Number of calling contexts
        ///
        uint32_t
        numContexts() const;

        /// @brief Get calling context by ID.
        ///
        /// Returns a pointer to the calling context definition stored with
        /// the given identifier @a id in the range [0,@#contexts-1].
        ///
        /// @param id
        ///     Calling context identifier
        /// @returns
        ///     Pointer to the requested calling context
        ///
        CallingContext*
        getContext(CallingContext::IdType id) const;

        /// @brief Add a calling context object.
        ///
        /// Adds the given calling context definition object @a context to the
        /// calling context tree.  Valid arguments are only non-`nullptr`s to
        /// CallingContext objects with subsequent IDs, i.e., the identifier
        /// of the given @a context has to match the value returned by
        /// numContexts() before the call.
        ///
        /// @param context
        ///     Calling context to be added
        ///
        void
        addContext(CallingContext* context);

        /// @}


    private:
        //--- Type definitions -----------------------------

        /// Container type for calling contexts
        typedef std::vector<CallingContext*> ContextContainer;


        //--- Data members ---------------------------------

        /// Calling context definitions: ID |-@> calling context
        ContextContainer mContexts;


        //--- Private methods & friends --------------------

        std::ostream&
        print(std::ostream& stream) const;

        friend std::ostream&
        operator<<(std::ostream&      stream,
                   const ContextTree& item);
};


//--- Related functions -----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates ContextTree
///
/// Prints the contents of the given calling context tree @a item to an
/// output @a stream in a human-readable form.
///
/// @param stream
///     Output stream
/// @param item
///     Calling context tree object
/// @returns
///     Output stream
///
std::ostream&
operator<<(std::ostream&      stream,
           const ContextTree& item);

/// @}
}   // namespace detail
}   // namespace pearl


#endif   // !PEARL_CONTEXTTREE_H
