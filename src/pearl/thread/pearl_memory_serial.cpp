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
 *  @ingroup PEARL_thread
 *  @brief   Implementation of custom memory management functions for
 *           single-threaded codes.
 *
 *  This header file provides the implementation of the PEARL-internal
 *  custom memory management functions for single-threaded codes.  See
 *  pearl_memory.h for a detailed description of the API.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "pearl_memory.h"

#include <cassert>

#include "SmallBlockAllocator.h"

using namespace std;
using namespace pearl::detail;


//--- Internal declarations -------------------------------------------------

namespace
{
/// Single allocator used by the memory management
SmallBlockAllocator* allocator = 0;
}   // unnamed namespace


//--- Memory management: Environmental management ---------------------------

void
pearl::detail::memoryFinalize()
{
    delete allocator;
    allocator = 0;
}


//--- Memory management: Allocation routines --------------------------------

void*
pearl::detail::memoryAllocate(const std::size_t blockSize)
{
    if (allocator == 0)
        allocator = new SmallBlockAllocator;

    return allocator->allocate(blockSize);
}


void
pearl::detail::memoryDeallocate(void* const       ptr,
                                const std::size_t blockSize)
{
    assert(allocator);

    allocator->deallocate(ptr, blockSize);
}
