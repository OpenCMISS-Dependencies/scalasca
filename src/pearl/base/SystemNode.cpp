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


#include <config.h>
#include <pearl/SystemNode.h>

#include <cassert>
#include <cstddef>
#include <iostream>

#include <pearl/LocationGroup.h>
#include <pearl/String.h>

#include "pearl_iomanip.h"

using namespace std;
using namespace pearl;


/*-------------------------------------------------------------------------*/
/**
 *  @file    SystemNode.cpp
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class SystemNode.
 *
 *  This file provides the implementation of the class SystemNode and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


//--- Constructors & destructor ---------------------------------------------

SystemNode::SystemNode(const IdType      id,
                       const String&     name,
                       const String&     className,
                       SystemNode* const parent)
    : mIdentifier(id),
      mName(name),
      mClassName(className),
      mParent(parent)
{
    if (parent) {
        parent->mChildren.push_back(this);
    }
}


//--- Access definition data ------------------------------------------------

SystemNode::IdType
SystemNode::getId() const
{
    return mIdentifier;
}


const String&
SystemNode::getName() const
{
    return mName;
}


const String&
SystemNode::getClassName() const
{
    return mClassName;
}


SystemNode*
SystemNode::getParent() const
{
    return mParent;
}


uint32_t
SystemNode::numChildren() const
{
    return mChildren.size();
}


const SystemNode&
SystemNode::getChild(const uint32_t index) const
{
    return *(mChildren.at(index));
}


uint32_t
SystemNode::numLocationGroups() const
{
    return mLocationGroups.size();
}


const LocationGroup&
SystemNode::getLocationGroup(uint32_t index) const
{
    return *(mLocationGroups.at(index));
}


//--- Private methods -------------------------------------------------------

/// @brief Stream output helper function.
///
/// Prints the system node's name and the identifier to the given @a stream
/// before recursively processing all child nodes, including the referenced
/// location groups. This member function is triggered by SystemTree::print()
/// to print a nicely formatted system hierarchy.
///
/// @param  stream  Output stream
/// @param  prefix  Prefix printed before the node data
/// @param  isLast  True if the node is the last entry in the child list of
///                 its parent (including both system nodes and location
///                 groups), false otherwise.
///
/// @return Output stream
///
ostream&
SystemNode::print(ostream&      stream,
                  const string& prefix,
                  const bool    isLast) const
{
    // Print current system node
    stream << prefix
           << " + " << mName.getString()
           << "  <sn:" << mIdentifier << ">"
           << endl;

    // Process child nodes
    size_t count = mChildren.size();
    for (size_t index = 0; index < count; ++index) {
        mChildren[index]->print(stream,
                                prefix + (isLast ? "   " : " | "),
                                (count == (index + 1))
                                && mLocationGroups.empty());
    }

    // Process location groups
    count = mLocationGroups.size();
    for (size_t index = 0; index < count; ++index) {
        mLocationGroups[index]->print(stream,
                                      prefix + (isLast ? "   " : " | "),
                                      (count == (index + 1)));
    }
    return stream;
}


/// @brief Add a location group.
///
/// Attaches the given location group to the system node instance.
///
/// @param  group  %Location group to attach (non-NULL)
///
void
SystemNode::addLocationGroup(LocationGroup* const group)
{
    assert(group);
    mLocationGroups.push_back(group);
}


//--- Related functions -----------------------------------------------------

namespace pearl
{
//--- Stream I/O operators ---------------------------------

ostream&
operator<<(ostream&          stream,
           const SystemNode& item)
{
    // Special case: undefined system node
    if (SystemNode::UNDEFINED == item) {
        return stream << "<undefined>";
    }

    // Adjust identation
    int indent = getIndent(stream);
    setIndent(stream, indent + 11);

    // Print data
    stream << "SystemNode {" << iendl(indent)
           << "  id     = " << item.getId() << iendl(indent)
           << "  name   = " << item.getName() << iendl(indent)
           << "  class  = " << item.getClassName() << iendl(indent)
           << "  parent = ";
    SystemNode* parent = item.getParent();
    if (NULL == parent) {
        stream << "<undefined>";
    } else {
        stream << parent->getId();
    }
    stream << iendl(indent)
           << "}";

    // Reset indentation
    return setIndent(stream, indent);
}


//--- Comparison operators ---------------------------------

bool
operator==(const SystemNode& lhs,
           const SystemNode& rhs)
{
    return ((lhs.getId() == rhs.getId())
            && (lhs.getName() == rhs.getName())
            && (lhs.getClassName() == rhs.getClassName())
            && (lhs.getParent() == rhs.getParent()));
}


bool
operator!=(const SystemNode& lhs,
           const SystemNode& rhs)
{
    return !(lhs == rhs);
}
}   // namespace pearl
