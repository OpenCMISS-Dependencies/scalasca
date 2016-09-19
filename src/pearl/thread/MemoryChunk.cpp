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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_thread
 *  @brief   Implementation of the class pearl::detail::MemoryChunk.
 *
 *  This file provides the implementation of the internal helper class
 *  pearl::detail::MemoryChunk.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "MemoryChunk.h"

using namespace std;
using namespace pearl::detail;


//--- Internal declarations -------------------------------------------------

namespace
{
/// Size of additional redzone before each chunk when running under Valgrind
#if HAVE(VALGRIND_MEMCHECK_H)

const size_t REDZONE_OFFSET = 16;

#else   // !HAVE(VALGRIND_MEMCHECK_H)

const size_t REDZONE_OFFSET = 0;

#endif // !HAVE(VALGRIND_MEMCHECK_H)
}   // unnamed namespace


//--- Constructors & destructor ---------------------------------------------

MemoryChunk::MemoryChunk(std::size_t        chunkSize,
                         const std::size_t  blockSize,
                         const std::size_t  numBlocks,
                         MemoryChunk* const nextChunk)
    : mNextChunk(nextChunk),
      mAvailableBlocks(numBlocks),
      mFirstAvailableBlock(0)
{
    assert(blockSize > 0);
    assert(blockSize <= chunkSize);
    assert(numBlocks == (chunkSize / blockSize));
    assert(numBlocks <= 256);

    // Valgrind's memcheck tool gets confused if the pointers to the first
    // memory block and the chunk itself are identical.  Thus, we are adding
    // an additional redzone in front of the chunk.
    if (RUNNING_ON_VALGRIND != 0) {
        chunkSize += REDZONE_OFFSET;
    }

    // Allocate memory chunk:
    // Use chunkSize rather than (numBlocks * blockSize) to potentially allow
    // reuse of the memory chunk for a different block size after deallocation,
    // thereby reducing the probability of being hit by memory fragmentation.
    // Also, chunkSize includes the size of the additional redzone when running
    // under Valgrind.
    mData = new unsigned char[chunkSize];

    // Mark the entire chunk as unaccessible; adjust data pointer to point to
    // the first byte after the redzone
    VALGRIND_MAKE_MEM_NOACCESS(mData, chunkSize);
    if (RUNNING_ON_VALGRIND != 0) {
        mData += REDZONE_OFFSET;
    }

    // Initialize free list, tweaking Valgrind's accessibility flags to avoid
    // "invalid write" errors
    unsigned char        index     = 0;
    unsigned char* const lastBlock = mData + (numBlocks - 1) * blockSize;
    for (unsigned char* block = mData; block < lastBlock; block += blockSize) {
        VALGRIND_MAKE_MEM_UNDEFINED(block, sizeof(unsigned char));
        *block = ++index;
        VALGRIND_MAKE_MEM_NOACCESS(block, sizeof(unsigned char));
    }
}


MemoryChunk::~MemoryChunk()
{
    // Adjust data pointer to point to the first byte of the allocation
    if (RUNNING_ON_VALGRIND != 0) {
        mData -= REDZONE_OFFSET;
    }

    // Deallocate memory chunk
    delete[] mData;
}
