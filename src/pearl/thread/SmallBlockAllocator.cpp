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
 *  @brief   Implementation of the class pearl::detail::SmallBlockAllocator.
 *
 *  This file provides the implementation of the internal helper class
 *  pearl::detail::SmallBlockAllocator.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "SmallBlockAllocator.h"

#include <cassert>
#include <cstring>

#include "ChunkAllocator.h"

using namespace std;
using namespace pearl::detail;


//--- Internal declarations -------------------------------------------------

namespace
{
    /// Maximum memory block size handled by %SmallBlockAllocator
    const size_t MAX_BLOCK_SIZE = 64;

    /// Minimum alignment used by %SmallBlockAllocator
    const size_t MIN_ALIGNMENT = 4;
}   // unnamed namespace


//--- Constructors & destructor ---------------------------------------------

SmallBlockAllocator::SmallBlockAllocator()
{
    const size_t numAllocators = MAX_BLOCK_SIZE / MIN_ALIGNMENT;

    // Initialize allocator array
    mAllocators = new ChunkAllocator *[numAllocators];
    memset(mAllocators, 0, numAllocators * sizeof(ChunkAllocator*));
}


SmallBlockAllocator::~SmallBlockAllocator()
{
    // Delete chunk allocators
    const size_t numAllocators = MAX_BLOCK_SIZE / MIN_ALIGNMENT;
    for (size_t index = 0; index < numAllocators; ++index) {
        delete mAllocators[index];
    }

    // Delete allocator array
    delete[] mAllocators;
}


//--- Memory allocation routines --------------------------------------------

void*
SmallBlockAllocator::allocate(const std::size_t blockSize)
{
    // Let the default memory management handle "extreme" cases
    if ((blockSize == 0) || (blockSize > MAX_BLOCK_SIZE)) {
        return :: operator new(blockSize);
    }

    // Allocate from chunk allocator for block size; create it if necessary
    const size_t index = (blockSize + MIN_ALIGNMENT - 1) / MIN_ALIGNMENT;
    if (mAllocators[index] == 0) {
        mAllocators[index] = new ChunkAllocator(index * MIN_ALIGNMENT);
    }
    return mAllocators[index]->allocate();
}


void
SmallBlockAllocator::deallocate(void* const       ptr,
                                const std::size_t blockSize)
{
    // Ignore nullptr
    if (ptr == 0) {
        return;
    }

    // Let the default memory management handle "extreme" cases
    if ((blockSize == 0) || (blockSize > MAX_BLOCK_SIZE))
    {
        :: operator delete(ptr);

        return;
    }

    // Deallocate to chunk allocator for block size
    const size_t index = (blockSize + MIN_ALIGNMENT - 1) / MIN_ALIGNMENT;
    assert(mAllocators[index] != 0);
    mAllocators[index]->deallocate(ptr);
}
