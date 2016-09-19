/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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
#include "SystemTree.h"

#include <algorithm>
#include <cassert>
#include <cstddef>

#include <pearl/Error.h>

#include "Functors.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


/*-------------------------------------------------------------------------*/
/**
 *  @file    SystemTree.cpp
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class SystemTree.
 *
 *  This file provides the implementation of the class SystemTree and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


//--- Constructors & destructor ---------------------------------------------

SystemTree::SystemTree()
{
}


SystemTree::~SystemTree()
{
    for_each(mSystemNodes.begin(), mSystemNodes.end(),
             delete_ptr<SystemNode>());
    mSystemNodes.clear();
    mRootNodes.clear();
}


//--- Query/modify the system tree information ------------------------------

uint32_t
SystemTree::numSystemNodes() const
{
    return mSystemNodes.size();
}


SystemNode*
SystemTree::getSystemNode(const SystemNode::IdType id) const
{
    if (SystemNode::NO_ID == id) {
        return NULL;
    }
    return mSystemNodes.at(id);
}


void
SystemTree::addSystemNode(SystemNode* const node)
{
    // Something went wrong when called with a NULL pointer
    assert(node);

    // System nodes have to be inserted in order
    if (mSystemNodes.size() != node->getId()) {
        throw RuntimeError("SystemTree::addSystemNode(SystemNode*)"
                           " -- Invalid ID.");
    }

    // Store system node
    mSystemNodes.push_back(node);
    if (!node->getParent()) {
        mRootNodes.push_back(node);
    }
}


//--- Private methods -------------------------------------------------------

/// @brief Stream output helper function.
///
/// Prints the instance data to the given @a stream in a human-readable form.
/// It therefore is the low-level implementation of operator<<().
///
/// @param  stream  Output stream
///
/// @return Output stream
///
std::ostream&
SystemTree::print(std::ostream& stream) const
{
    // Print root nodes, which recursively print their children
    size_t count = mRootNodes.size();
    for (size_t index = 0; index < count; ++index) {
        mRootNodes[index]->print(stream, "", (count == (index + 1)));
    }
    return stream;
}


//--- Related functions -----------------------------------------------------

namespace pearl
{
    //--- Stream I/O operators ---------------------------------

    ostream&
    operator<<(ostream&          stream,
               const SystemTree& item)
    {
        return item.print(stream);
    }
}   // namespace pearl
