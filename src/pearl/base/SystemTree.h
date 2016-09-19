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


#ifndef PEARL_SYSTEMTREE_H
#define PEARL_SYSTEMTREE_H


#include <iosfwd>
#include <vector>

#include <stdint.h>

#include <pearl/SystemNode.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    SystemTree.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class SystemTree.
 *
 *  This header file provides the declaration of the class SystemTree and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   SystemTree
 *  @ingroup PEARL_base
 *  @brief   Container class for system hierarchies.
 *
 *  Instances of the SystemTree class store the data of the hardware-related
 *  global system hierarchy of the computing platform. The system tree
 *  consists of a set of SystemNode objects encoding the actual parent-child
 *  relationships.
 **/
/*-------------------------------------------------------------------------*/

class SystemTree
{
    public:
        //--- Public methods -------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Default constructor.
        ///
        /// Creates a new instance representing an empty system tree.
        ///
        SystemTree();

        /// @brief Destructor.
        ///
        /// Releases all allocated resources and destroys the instance.
        ///
        ~SystemTree();

        /// @}
        /// @name Query/modify the system tree information
        /// @{

        /// @brief Get number of tree nodes.
        ///
        /// Returns the total number of system nodes currently stored in the
        /// system tree.
        ///
        /// @return Number of tree nodes
        ///
        uint32_t
        numSystemNodes() const;

        /// @brief Get system node by ID.
        ///
        /// Returns a pointer to the system node object stored with the given
        /// identifier @a id in the range [0,@#system_nodes-1], or NULL if
        /// @a id equals to SystemNode::NO_ID.
        ///
        /// @param  id  System node identifier
        ///
        /// @return Pointer to requested system node or NULL
        ///
        SystemNode*
        getSystemNode(SystemNode::IdType id) const;

        /// @brief Add system node object.
        ///
        /// Adds the given system @a node to the system tree. Valid arguments
        /// are only non-NULL pointers to system node objects with subsequent
        /// IDs, i.e., the identifier of system @a node has to match the value
        /// returned by numNodes() before the call.
        ///
        /// @param  node  System node to be added
        ///
        void
        addSystemNode(SystemNode* node);

        /// @}


    private:
        //--- Data members ---------------------------------

        /// Container type for system nodes
        typedef std::vector<SystemNode*> NodeContainer;


        /// System node definitions: system node ID |-@> system node
        NodeContainer mSystemNodes;

        /// List of root nodes
        NodeContainer mRootNodes;


        //--- Private methods & friends --------------------

        std::ostream&
        print(std::ostream& stream) const;

        friend std::ostream&
        operator<<(std::ostream&     stream,
                   const SystemTree& item);
};


//--- Related functions -----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates SystemTree
///
/// Prints the contents of the given system tree @a item to an output
/// @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    System tree object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream&     stream,
           const SystemTree& item);

/// @}
}   // namespace pearl


#endif   // !PEARL_SYSTEMTREE_H
