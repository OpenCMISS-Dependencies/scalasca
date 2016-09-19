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
#include <pearl/ProcessGroup.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#include <pearl/Error.h>
#include <pearl/String.h>

#include "pearl_iomanip.h"

using namespace std;
using namespace pearl;


//--- Constants -------------------------------------------------------------

static unsigned char GROUP_FLAG_SELF  = 1;
static unsigned char GROUP_FLAG_WORLD = 2;


/*-------------------------------------------------------------------------*/
/**
 *  @file    ProcessGroup.cpp
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class ProcessGroup.
 *
 *  This file provides the implementation of the class ProcessGroup and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


//--- Constructors & destructor ---------------------------------------------

ProcessGroup::ProcessGroup(const IdType         id,
                           const String&        name,
                           const RankContainer& ranks,
                           const bool           isSelf,
                           const bool           isWorld)
    : mIdentifier(id),
      mName(name),
      mRanks(ranks),
      mFlags(0)
{
    assert(!ranks.empty()
           || (isSelf && isWorld));

    // Set group flags
    if (isSelf) {
        mFlags |= GROUP_FLAG_SELF;
    }
    if (isWorld) {
        mFlags |= GROUP_FLAG_WORLD;
    }
}


ProcessGroup::~ProcessGroup()
{
}


//--- Access definition data ------------------------------------------------

ProcessGroup::IdType
ProcessGroup::getId() const
{
    return mIdentifier;
}


const String&
ProcessGroup::getName() const
{
    return mName;
}


uint32_t
ProcessGroup::numRanks() const
{
    return mRanks.size();
}


int
ProcessGroup::getLocalRank(const int globalRank) const
{
    // Special case: for WORLD groups, there is a 1:1 rank mapping
    if (isWorld()) {
        if (static_cast<size_t>(globalRank) >= mRanks.size()) {
            throw RuntimeError("ProcessGroup::getLocalRank(int)"
                               " -- Unknown identifier.");
        }
        return globalRank;
    }

    // Search process identifier. This has to be a linear search since members
    // are not ordered by their global process IDs.
    RankContainer::const_iterator it;
    it = find(mRanks.begin(), mRanks.end(), globalRank);

    // Not found...
    if (mRanks.end() == it) {
        throw RuntimeError("ProcessGroup::getLocalRank(int)"
                           " -- Unknown identifier.");
    }

    // Found => Return local rank
    return it - mRanks.begin();
}


int
ProcessGroup::getGlobalRank(const int localRank) const
{
    if (static_cast<size_t>(localRank) >= mRanks.size()) {
        throw RuntimeError("ProcessGroup::getGlobalRank(int)"
                           " -- Rank out of range.");
    }
    return mRanks[localRank];
}


bool
ProcessGroup::isSelf() const
{
    return (mFlags & GROUP_FLAG_SELF);
}


bool
ProcessGroup::isWorld() const
{
    return (mFlags & GROUP_FLAG_WORLD);
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
ostream&
ProcessGroup::print(ostream& stream) const
{
    // Adjust indentation
    int indent = getIndent(stream);
    setIndent(stream, indent + 9);

    // Print data
    stream << "ProcessGroup {" << iendl(indent)
           << "  id   = " << getId() << iendl(indent)
           << "  name = " << getName() << iendl(indent)
           << "}";

    // Reset indentation
    return setIndent(stream, indent);
}


//--- Related functions -----------------------------------------------------

namespace pearl
{
//--- Stream I/O operators ---------------------------------

ostream&
operator<<(ostream&            stream,
           const ProcessGroup& item)
{
    // Special case: undefined process group
    if (ProcessGroup::UNDEFINED == item) {
        return stream << "<undefined>";
    }
    return item.print(stream);
}


//--- Comparison operators ---------------------------------

bool
operator==(const ProcessGroup& lhs,
           const ProcessGroup& rhs)
{
    return ((lhs.getId() == rhs.getId())
            && (lhs.getName() == rhs.getName())
            && (lhs.numRanks() == rhs.numRanks()));
}


bool
operator!=(const ProcessGroup& lhs,
           const ProcessGroup& rhs)
{
    return !(lhs == rhs);
}
}   // namespace pearl
