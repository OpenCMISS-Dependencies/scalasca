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


#include <config.h>
#include "ChunkAllocator.h"

#include <algorithm>
#include <cassert>
#include <cstddef>

#include <gtest/gtest.h>

using namespace std;
using namespace testing;
using namespace pearl::detail;


namespace
{
//--- Constants -------------------------------------------------------------

// Maximum number of blocks to allocate; this fills at least 8 chunks
static size_t MAX_BLOCKS = 2048;

}   // unnamed namespace


//--- ChunkAllocator tests --------------------------------------------------

// Test fixture used for chunk allocator tests
class ChunkAllocatorT
    : public TestWithParam<size_t>
{
    public:
        // Constructors & destructor
        ChunkAllocatorT()
        {
            const size_t& blockSize = GetParam();

            mAllocator = new ChunkAllocator(blockSize);
            mPointers.reserve(MAX_BLOCKS);
        }


        ~ChunkAllocatorT()
        {
            delete mAllocator;
        }


        // Helper functions
        void
        allocate(size_t count);
        void
        deallocFIFO();
        void
        deallocLIFO();
        void
        deallocChunk();
        void
        deallocStride();
        void
        checkUniqueness(size_t count);


    protected:
        ChunkAllocator* mAllocator;
        vector<void*>   mPointers;
};


void
ChunkAllocatorT::allocate(const size_t count)
{
    assert(mPointers.empty());

    for (size_t i = 0; i < count; ++i) {
        void* ptr = mAllocator->allocate();
        EXPECT_TRUE(0 != ptr);
        mPointers.push_back(ptr);
    }
}


void
ChunkAllocatorT::deallocFIFO()
{
    const size_t count = mPointers.size();
    for (size_t i = 0; i < count; ++i) {
        mAllocator->deallocate(mPointers[i]);
    }
    mPointers.clear();
}


void
ChunkAllocatorT::deallocLIFO()
{
    while (!mPointers.empty()) {
        mAllocator->deallocate(mPointers.back());
        mPointers.pop_back();
    }
}


void
ChunkAllocatorT::deallocChunk()
{
    const size_t count = mPointers.size();
    const size_t lb = count / 4;
    const size_t ub = (count * 3) / 4;

    for (size_t i = lb; i < ub; ++i) {
        mAllocator->deallocate(mPointers[i]);
    }
    for (size_t i = 0; i < lb; ++i) {
        mAllocator->deallocate(mPointers[i]);
    }
    for (size_t i = ub; i < count; ++i) {
        mAllocator->deallocate(mPointers[i]);
    }
    mPointers.clear();
}


void
ChunkAllocatorT::deallocStride()
{
    const size_t count = mPointers.size();

    size_t index = (6 % count);
    for (size_t i = 0; i < count; ++i) {
        mAllocator->deallocate(mPointers[index]);

        index = (index + 13) % count;
    }
    mPointers.clear();
}


void
ChunkAllocatorT::checkUniqueness(const size_t count)
{
    vector<void*> sortedPtrs(mPointers);
    sort(sortedPtrs.begin(), sortedPtrs.end());
    sortedPtrs.erase(unique(sortedPtrs.begin(), sortedPtrs.end()), sortedPtrs.end());
    EXPECT_EQ(count, sortedPtrs.size());
}


TEST_P(ChunkAllocatorT, testAllocDeallocFIFO) {
    // Allocate/deallocate a "large" number of blocks, filling multiple
    // chunks.  Verify that
    //  - all returned pointers are non-NULL
    //  - all pointers are unique
    //  - deallocation in FIFO order works
    {
        SCOPED_TRACE("First allocation");
        allocate(MAX_BLOCKS);
        checkUniqueness(MAX_BLOCKS);
        deallocFIFO();
    }

    // Allocate/deallocate blocks again
    {
        SCOPED_TRACE("Second allocation");
        allocate(MAX_BLOCKS);
        checkUniqueness(MAX_BLOCKS);
        deallocFIFO();
    }
}


TEST_P(ChunkAllocatorT, testAllocDeallocLIFO) {
    // Allocate/deallocate a "large" number of blocks, filling multiple
    // chunks.  Verify that
    //  - all returned pointers are non-NULL
    //  - all pointers are unique
    //  - deallocation in LIFO order works
    {
        SCOPED_TRACE("First allocation");
        allocate(MAX_BLOCKS);
        checkUniqueness(MAX_BLOCKS);
        deallocLIFO();
    }

    // Allocate/deallocate blocks again
    {
        SCOPED_TRACE("Second allocation");
        allocate(MAX_BLOCKS);
        checkUniqueness(MAX_BLOCKS);
        deallocLIFO();
    }
}


TEST_P(ChunkAllocatorT, testAllocDeallocChunk) {
    // Allocate/deallocate a "large" number of blocks, filling multiple
    // chunks.  Verify that
    //  - all returned pointers are non-NULL
    //  - all pointers are unique
    //  - deallocation in "chunk order" (first middle half, then lower quarter,
    //    then upper quarter) works
    {
        SCOPED_TRACE("First allocation");
        allocate(MAX_BLOCKS);
        checkUniqueness(MAX_BLOCKS);
        deallocChunk();
    }

    // Allocate/deallocate blocks again
    {
        SCOPED_TRACE("Second allocation");
        allocate(MAX_BLOCKS);
        checkUniqueness(MAX_BLOCKS);
        deallocChunk();
    }
}


TEST_P(ChunkAllocatorT, testAllocDeallocStride) {
    // Allocate/deallocate a "large" number of blocks, filling multiple
    // chunks.  Verify that
    //  - all returned pointers are non-NULL
    //  - all pointers are unique
    //  - deallocation in non-consecutive order (start index 6, stride 13)
    //    works
    {
        SCOPED_TRACE("First allocation");
        allocate(MAX_BLOCKS);
        checkUniqueness(MAX_BLOCKS);
        deallocStride();
    }

    // Allocate/deallocate blocks again
    {
        SCOPED_TRACE("Second allocation");
        allocate(MAX_BLOCKS);
        checkUniqueness(MAX_BLOCKS);
        deallocStride();
    }
}


TEST_P(ChunkAllocatorT, testReplace) {
    // Allocate a "large" number of blocks, filling multiple chunks
    allocate(MAX_BLOCKS);

    // Deallocate each block in FIFO order and allocate a new one; pointers
    // should match
    const size_t count = mPointers.size();
    for (size_t i = 0; i < count; ++i) {
        void* oldPtr = mPointers[i];

        mAllocator->deallocate(oldPtr);
        void* newPtr = mAllocator->allocate();

        EXPECT_EQ(oldPtr, newPtr);

        // Update entry in case we got a different pointer
        mPointers[i] = newPtr;
    }

    // Cleanup
    deallocFIFO();
}


// Instantiate tests with various block sizes, especially with the boundary
// values triggering different chunk sizes
INSTANTIATE_TEST_CASE_P(ChunkAllocator,
                        ChunkAllocatorT,
                        Values(1, 3, 4, 15, 16, 32));
