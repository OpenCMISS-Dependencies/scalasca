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


#include <config.h>
#include <pearl/LocationGroup.h>

#include <cassert>
#include <iostream>

#include <pearl/Location.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>

using namespace std;
using namespace pearl;


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class LocationGroup.
 *
 *  This file provides the implementation of the class LocationGroup and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


//--- Static class data -----------------------------------------------------

const LocationGroup::IdType LocationGroup::NO_ID;


//--- Constructors & destructor ---------------------------------------------

LocationGroup::LocationGroup(const IdType      id,
                             const String&     name,
                             SystemNode* const parent)
    : mIdentifier(id),
      mName(name),
      mParent(parent)
{
    // Dangling location groups are prohibited
    assert(parent);

    parent->addLocationGroup(this);
}


LocationGroup::~LocationGroup()
{
}


//--- Access definition data ------------------------------------------------

LocationGroup::IdType
LocationGroup::getId() const
{
    return mIdentifier;
}


const String&
LocationGroup::getName() const
{
    return mName;
}


SystemNode*
LocationGroup::getParent() const
{
    return mParent;
}


uint32_t
LocationGroup::numLocations() const
{
    return mLocations.size();
}


const Location&
LocationGroup::getLocation(const uint32_t index) const
{
    return *(mLocations.at(index));
}


//--- Private methods -------------------------------------------------------

/// @brief Stream output helper function.
///
/// Prints the location group's name and its identifier to the given
/// @a stream before recursively processing all child locations. This
/// member function is triggered by SystemNode::print() to print a nicely
/// formatted system hierarchy.
///
/// @param  stream  Output stream
/// @param  prefix  Prefix printed before the location group data
/// @param  isLast  True if the location group is the last entry in the group
///                 list of its parent (a system node), false otherwise.
///
/// @return Output stream
///
ostream&
LocationGroup::print(ostream&      stream,
                     const string& prefix,
                     const bool    isLast) const
{
    // Print current location group
    stream << prefix
           << " + " << mName.getString()
           << "  <lg:" << mIdentifier << ">"
           << endl;

    // Process locations
    size_t count = mLocations.size();
    for (size_t index = 0; index < count; ++index) {
        mLocations[index]->print(stream,
                                 prefix + (isLast ? "   " : " | "),
                                 (count == (index + 1)));
    }
    return stream;
}


/// @brief Add a location.
///
/// Attaches the given @a location to the location group instance.
///
/// @param  location  %Location to attach (non-NULL)
///
void
LocationGroup::addLocation(Location* const location)
{
    assert(location);
    mLocations.push_back(location);
}


//--- Related functions -----------------------------------------------------

namespace pearl
{
//--- Stream I/O operators ---------------------------------

ostream&
operator<<(ostream&             stream,
           const LocationGroup& item)
{
    return item.output(stream);
}


//--- Comparison operators ---------------------------------

bool
operator==(const LocationGroup& lhs,
           const LocationGroup& rhs)
{
    return ((lhs.getId() == rhs.getId())
            && (lhs.getName() == rhs.getName())
            && (lhs.getType() == rhs.getType())
            && (lhs.getParent() == rhs.getParent()));
}


bool
operator!=(const LocationGroup& lhs,
           const LocationGroup& rhs)
{
    return !(lhs == rhs);
}
}   // namespace pearl
