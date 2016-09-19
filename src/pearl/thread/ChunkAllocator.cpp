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
 *  @brief   Implementation of the class pearl::detail::ChunkAllocator.
 *
 *  This file provides the implementation of the internal helper class
 *  pearl::detail::ChunkAllocator.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "ChunkAllocator.h"

#include <cassert>

#include "MemoryChunk.h"

using namespace std;
using namespace pearl::detail;


//--- Internal declarations -------------------------------------------------

namespace
{
    // *INDENT-OFF*
    size_t getChunkSize(size_t blockSize);
    // *INDENT-ON*
}   // unnamed namespace


//--- Constructors & destructor ---------------------------------------------

ChunkAllocator::ChunkAllocator(const std::size_t blockSize)
    : mChunkListHead(0),
      mChunkListTail(0),
      mEmptyCache(0),
      mAllocCache(0),
      mDeallocCache(0),
      mDeallocCachePrev(0),
      mBlockSize(blockSize),
      mAvailableBlocks(0)
{
    assert(blockSize > 0);
    assert(blockSize <= 4096);
}


ChunkAllocator::~ChunkAllocator()
{
    // Release chunks in list
    while (mChunkListHead != 0) {
        MemoryChunk* chunk = mChunkListHead;
        mChunkListHead = mChunkListHead->getNextChunk();
        delete chunk;
    }

    // The empty chunk cache may hold another instance
    delete mEmptyCache;
}


//--- Memory allocation routines --------------------------------------------

void*
ChunkAllocator::allocate()
{
    // All blocks occupied => new chunk
    if (mAvailableBlocks == 0) {
        const size_t chunkSize = getChunkSize(mBlockSize);
        const size_t numBlocks = chunkSize / mBlockSize;

        // Use cached empty chunk if available, create a new one otherwise
        if (mEmptyCache != 0) {
            mAllocCache = mEmptyCache;
            mEmptyCache = 0;
        } else {
            mAllocCache = new MemoryChunk(chunkSize, mBlockSize, numBlocks);
        }
        mAvailableBlocks = numBlocks;

        // Append chunk and update list tail
        if (mChunkListTail != 0) {
            mChunkListTail->setNextChunk(mAllocCache);
        }
        mChunkListTail = mAllocCache;

        // First chunk => update list head
        if (mChunkListHead == 0) {
            mChunkListHead = mAllocCache;
        }
    }
    // Search for chunk with an available block
    else {
        assert(mChunkListHead != 0);

        // If allocation cache is unset, start at the head of the chunk list
        // as we are assuming a FIFO deallocation strategy, i.e., unused
        // entries are more likely to be found at the beginning of the list.
        if (mAllocCache == 0) {
            mAllocCache = mChunkListHead;
        }

        // Start search with the current allocation cache.  As it may be a
        // chunk in the middle of the list, wrap around at the end.
        MemoryChunk* const stopChunk = mAllocCache;
        do {
            if (mAllocCache->availableBlocks() > 0) {
                break;
            }
            mAllocCache = mAllocCache->getNextChunk();
            if (mAllocCache == 0) {
                mAllocCache = mChunkListHead;
            }
        } while (mAllocCache != stopChunk);
    }

    // The allocation cache now points to a chunk with an available block.
    // If not, the allocator data is corrupt.
    assert(mAllocCache->availableBlocks() > 0);

    // Allocate from cache and update available block count
    void* const ptr = mAllocCache->allocate(mBlockSize);
    assert(ptr != 0);
    --mAvailableBlocks;

    return ptr;
}


void
ChunkAllocator::deallocate(void* const ptr)
{
    assert(ptr != 0);
    assert(mChunkListHead != 0);

    // If deallocation cache is unset, choose the head of the chunk list,
    // as we are assuming a FIFO deallocation strategy
    if (mDeallocCache == 0) {
        mDeallocCache     = mChunkListHead;
        mDeallocCachePrev = 0;
    }

    // Search for the chunk containing the block pointer, starting with the
    // current deallocation cache.  As it may be a chunk in the middle of
    // the list, wrap around at the end.
    const size_t       chunkSize = getChunkSize(mBlockSize);
    MemoryChunk* const stopChunk = mDeallocCache;
    do {
        if (mDeallocCache->contains(ptr, chunkSize)) {
            break;
        }
        mDeallocCachePrev = mDeallocCache;
        mDeallocCache     = mDeallocCache->getNextChunk();
        if (mDeallocCache == 0) {
            mDeallocCache     = mChunkListHead;
            mDeallocCachePrev = 0;
        }
    } while (mDeallocCache != stopChunk);

    // The deallocation cache now points to the chunk containing the block
    // pointer.  If not, either the allocator data is corrupt or the pointer
    // was not created by this allocator instance.
    assert(mDeallocCache->contains(ptr, chunkSize));

    // Deallocate to cache and update available block count
    mDeallocCache->deallocate(ptr, mBlockSize);
    ++mAvailableBlocks;

    // Release chunk if it is empty
    const size_t numBlocks = chunkSize / mBlockSize;
    if (mDeallocCache->availableBlocks() == numBlocks) {
        // Update list tail if last element is removed
        if (mChunkListTail == mDeallocCache) {
            mChunkListTail = mDeallocCachePrev;
        }

        // Update
        //  - list head if first element is removed
        //  - next chunk pointer of previous element otherwise
        MemoryChunk* const nextChunk = mDeallocCache->getNextChunk();
        if (mChunkListHead == mDeallocCache) {
            mChunkListHead = nextChunk;
        } else {
            assert(mDeallocCachePrev != 0);
            mDeallocCachePrev->setNextChunk(nextChunk);
        }

        // Cache empty chunk for reuse; deallocate if another chunk is
        // already cached; adjust available block count
        if (mEmptyCache == 0) {
            mEmptyCache = mDeallocCache;
            mEmptyCache->setNextChunk(0);
        } else {
            delete mDeallocCache;
        }
        mAvailableBlocks -= numBlocks;

        // Unset allocation cache if corresponding chunk was removed
        if (mAllocCache == mDeallocCache) {
            mAllocCache = 0;
        }

        // Assuming a FIFO deallocation strategy, blocks from the follow-up
        // chunk are likely to be deallocated next
        mDeallocCache = nextChunk;
    }
    // Otherwise, update the allocation cache (for efficient replacement)
    else {
        mAllocCache = mDeallocCache;
    }
}


//--- Helper functions ------------------------------------------------------

namespace
{
/// @brief Determine chunk size for a given memory block size.
///
/// Returns the chunk size (in bytes) that is used for a memory block size
/// of @a blockSize bytes by %ChunkAllocator instances.
///
/// @param blockSize
///     Size of the memory block in bytes
/// @returns
///     Corresponding chunk size in bytes
///
inline size_t
getChunkSize(const size_t blockSize)
{
    if (blockSize < 4) {
        return 256;
    } else if (blockSize < 16) {
        return 1024;
    }
    return 4096;
}
}   // unnamed namespace
