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
 *  @brief   Implementation of the class ContextTree.
 *
 *  This file provides the implementation of the class ContextTree and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "ContextTree.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include <pearl/Error.h>

#include "Functors.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


//--- Constructors & destructor ---------------------------------------------

ContextTree::~ContextTree()
{
    for_each(mContexts.begin(), mContexts.end(), delete_ptr<CallingContext>());
}


//--- Query/modify the calling context tree information ---------------------

uint32_t
ContextTree::numContexts() const
{
    return mContexts.size();
}


CallingContext*
ContextTree::getContext(const CallingContext::IdType id) const
{
    return mContexts.at(id);
}


void
ContextTree::addContext(CallingContext* const context)
{
    // Something went wrong when called with a `nullptr`
    assert(context);

    // Calling contexts have to be inserted in order
    if (context->getId() != mContexts.size()) {
        throw RuntimeError("ContextTree::addContext(CallingContext*) -- "
                           "Invalid ID.");
    }

    mContexts.push_back(context);
}


//--- Private methods -------------------------------------------------------

/// @brief Stream output helper function.
///
/// Prints the instance data to the given @a stream in a human-readable form.
/// It is the low-level implementation of operator<<().
///
/// @param stream
///     Output stream
/// @returns
///     Output stream
///
std::ostream&
ContextTree::print(std::ostream& stream) const
{
    // Collect root calling contexts
    vector<CallingContext*> rootContexts;
    const size_t            numContexts = mContexts.size();
    for (size_t index = 0; index < numContexts; ++index) {
        CallingContext* context = mContexts[index];

        if (context->getParent() == 0) {
            rootContexts.push_back(context);
        }
    }

    // Print root calling contexts, which recursively print their children
    const size_t numRoots = rootContexts.size();
    for (size_t index = 0; index < numRoots; ++index) {
        rootContexts[index]->print(stream, "", (numRoots == (index + 1)));
    }

    return stream;
}


//--- Related functions -----------------------------------------------------

namespace pearl
{
namespace detail
{
//--- Stream I/O operators ---------------------------------

ostream&
operator<<(ostream&           stream,
           const ContextTree& item)
{
    return item.print(stream);
}
}   // namespace detail
}   // namespace pearl
