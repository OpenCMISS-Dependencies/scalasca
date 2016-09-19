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
 *  @brief   Implementation of the class Communicator.
 *
 *  This file provides the implementation of the class Communicator and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include <pearl/Communicator.h>

#include <cstddef>
#include <iostream>

#include <pearl/ProcessGroup.h>
#include <pearl/String.h>

#include "pearl_iomanip.h"

using namespace std;
using namespace pearl;


//--- Constructors & destructor ---------------------------------------------

Communicator::Communicator(const IdType        id,
                           const String&       name,
                           const ProcessGroup& group,
                           const Paradigm      paradigm,
                           Communicator* const parent)
    : mIdentifier(id),
      mName(name),
      mGroup(group),
      mParadigm(paradigm),
      mParent(parent)
{
}


Communicator::~Communicator()
{
}


//--- Access definition data ------------------------------------------------

Communicator::IdType
Communicator::getId() const
{
    return mIdentifier;
}


const String&
Communicator::getName() const
{
    return mName;
}


const ProcessGroup&
Communicator::getGroup() const
{
    return mGroup;
}


Communicator*
Communicator::getParent() const
{
    return mParent;
}


Paradigm
Communicator::getParadigm() const
{
    return mParadigm;
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
Communicator::print(ostream& stream) const
{
    // Adjust indentation
    int indent = getIndent(stream);
    setIndent(stream, indent + 9);

    // Print data
    stream << "Communicator {" << iendl(indent)
           << "  id       = " << getId() << iendl(indent)
           << "  name     = " << getName() << iendl(indent)
           << "  paradigm = " << getParadigm() << iendl(indent)
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
           const Communicator& item)
{
    // Special case: undefined process group
    if (Communicator::UNDEFINED == item) {
        return stream << "<undefined>";
    }
    return item.print(stream);
}


//--- Comparison operators ---------------------------------

bool
operator==(const Communicator& lhs,
           const Communicator& rhs)
{
    return ((lhs.getId() == rhs.getId())
            && (lhs.getName() == rhs.getName())
            && (lhs.getGroup() == rhs.getGroup())
            && (lhs.getParent() == rhs.getParent()));
}


bool
operator!=(const Communicator& lhs,
           const Communicator& rhs)
{
    return !(lhs == rhs);
}
}   // namespace pearl
