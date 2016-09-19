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
#include "Calltree.h"

#include <algorithm>
#include <cassert>
#include <cstddef>

#include <pearl/Buffer.h>
#include <pearl/Callsite.h>
#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/Region.h>

#include "CallpathMatcher.h"
#include "Functors.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


/*-------------------------------------------------------------------------*/
/**
 *  @file    Calltree.cpp
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class Calltree.
 *
 *  This file provides the implementation of the class Calltree and related
 *  functions.
 **/
/*-------------------------------------------------------------------------*/


//--- Constructors & destructor ---------------------------------------------

Calltree::Calltree()
    : mModified(false)
{
}


/// @todo Replace get_uint32() calls by more generic version
Calltree::Calltree(const GlobalDefs& defs,
                   Buffer&           buffer)
    : mModified(false)
{
    try {
        uint32_t count = buffer.get_uint32();
        for (uint32_t id = 0; id < count; ++id) {
            uint32_t regionId   = buffer.get_uint32();
            uint32_t callsiteId = buffer.get_uint32();
            uint32_t parentId   = buffer.get_uint32();

            addCallpath(new Callpath(id,
                                     defs.getRegion(regionId),
                                     defs.getCallsite(callsiteId),
                                     getCallpath(parentId)));
        }
    }
    catch (...) {
        clear();
        throw;
    }
}


Calltree::~Calltree()
{
    clear();
}


//--- Query/modify the calltree information ---------------------------------

uint32_t
Calltree::numCallpaths() const
{
    return mCallpaths.size();
}


Callpath*
Calltree::getCallpath(const Callpath::IdType id) const
{
    if (Callpath::NO_ID == id) {
        return NULL;
    }
    return mCallpaths.at(id);
}


Callpath*
Calltree::getCallpath(const Region&   region,
                      const Callsite& callsite,
                      Callpath* const parent)
{
    // Check whether callpath exists
    Callpath* callpath = findCallpath(region, callsite, parent);
    if (callpath) {
        return callpath;
    }

    // Create & add new callpath otherwise
    callpath = new Callpath(mCallpaths.size(), region, callsite, parent);
    addCallpath(callpath);

    return callpath;
}


void
Calltree::addCallpath(Callpath* const callpath)
{
    // Something went wrong when called with a NULL pointer
    assert(callpath);

    // Callpaths have to be inserted in order
    if (mCallpaths.size() != callpath->getId()) {
        throw RuntimeError("Calltree::addCallpath(Callpath*) -- Invalid ID.");
    }

    // Store callpath & update modification flag
    mCallpaths.push_back(callpath);
    if (!callpath->getParent()) {
        mRootCallpaths.push_back(callpath);
    }
    mModified = true;
}


//--- Merging of calltrees --------------------------------------------------

void
Calltree::merge(const Calltree& calltree)
{
    // Determine number of callpaths in merge tree
    uint32_t count = calltree.numCallpaths();

    // Allocate identifier mapping table since callpath IDs in the merge
    // tree have to be mapped to Callpath objects in the merged tree.
    vector<Callpath::IdType> idmap;
    idmap.reserve(count);

    // Process callpaths
    for (uint32_t id = 0; id < count; ++id) {
        Callpath* current = calltree.getCallpath(id);
        Callpath* parent  = current->getParent();
        Callpath* path    = getCallpath(current->getRegion(),
                                        current->getCallsite(),
                                        ((NULL == parent)
                                      ? NULL
                                      : mCallpaths[idmap[parent->getId()]]));

        idmap.push_back(path->getId());
    }
}


//--- Serialize calltree data -----------------------------------------------

/// @todo Replace put_uint32() calls by more generic version
void
Calltree::pack(Buffer& buffer) const
{
    uint32_t count = mCallpaths.size();

    buffer.put_uint32(count);
    for (uint32_t id = 0; id < count; ++id) {
        Callpath*       current  = mCallpaths[id];
        Callpath*       parent   = current->getParent();
        const Region&   region   = current->getRegion();
        const Callsite& callsite = current->getCallsite();

        buffer.put_uint32(region.getId());
        buffer.put_uint32(callsite.getId());
        buffer.put_uint32(parent ? parent->getId() : Callpath::NO_ID);
    }
}


//--- Private methods -------------------------------------------------------

/// @brief Releases all resources.
///
/// Releases all dynamically allocated resources (e.g., callpaths) and resets
/// all attributes to their default settings.
///
void
Calltree::clear()
{
    for_each(mCallpaths.begin(), mCallpaths.end(), delete_ptr<Callpath>());
    mCallpaths.clear();
    mRootCallpaths.clear();
    mModified = false;
}


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
Calltree::print(std::ostream& stream) const
{
    // Print root callpaths, which recursively print their children
    size_t count = mRootCallpaths.size();
    for (size_t index = 0; index < count; ++index) {
        mRootCallpaths[index]->print(stream, "", (count == (index + 1)));
    }
    return stream;
}


/// @brief Search a particular callpath.
///
/// Returns the pointer to the callpath object referencing the given @a region
/// and @a callsite of the @a parent callpath (NULL for root callpaths), or
/// NULL if no such callpath exists.
///
/// @param  region    Called region
/// @param  callsite  Specific callsite (Callsite::UNDEFINED if not applicable)
/// @param  parent    Parent callpath (NULL for root callpaths)
///
/// @return Pointer to callpath object if existent, NULL otherwise
///
Callpath*
Calltree::findCallpath(const Region&   region,
                       const Callsite& callsite,
                       Callpath* const parent) const
{
    // Has parent -> Search in child list
    if (parent) {
        return parent->findChild(region, callsite);
    }

    // No parent -> Search in list of root callpaths
    CallpathContainer::const_iterator it = find_if(mRootCallpaths.begin(),
                                                   mRootCallpaths.end(),
                                                   CallpathMatcher(region,
                                                                   callsite));
    if (mRootCallpaths.end() == it) {
        return NULL;
    }
    return *it;
}


//--- Related functions -----------------------------------------------------

namespace pearl
{
    //--- Stream I/O operators ---------------------------------

    ostream&
    operator<<(ostream&        stream,
               const Calltree& item)
    {
        return item.print(stream);
    }
}   // namespace pearl
