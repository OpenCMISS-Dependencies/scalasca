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
#include "MemoryChunk.h"

#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

using namespace std;
using namespace testing;
using namespace pearl::detail;


namespace
{
//--- Helpers ---------------------------------------------------------------

struct ChunkConfig
{
    ChunkConfig(const size_t chunkSize,
                const size_t blockSize)
        : mChunkSize(chunkSize),
          mBlockSize(blockSize),
          mNumBlocks(chunkSize / blockSize)
    {
    }


    size_t mChunkSize;
    size_t mBlockSize;
    size_t mNumBlocks;
};


ostream&
operator<<(ostream&           stream,
           const ChunkConfig& cfg)
{
    return stream << "{chunkSize=" << cfg.mChunkSize
           << ",blockSize=" << cfg.mBlockSize
           << ",numBlocks=" << cfg.mNumBlocks
           << "}";
}
}   // unnamed namespace


//--- MemoryChunk tests -----------------------------------------------------

// Test fixture used for memory chunk tests
class MemoryChunkT
    : public TestWithParam<ChunkConfig>
{
    public:
        MemoryChunkT()
        {
            const ChunkConfig& cfg = GetParam();

            mChunk = new MemoryChunk(cfg.mChunkSize,
                                     cfg.mBlockSize,
                                     cfg.mNumBlocks);
        }


        ~MemoryChunkT()
        {
            delete mChunk;
        }


    protected:
        MemoryChunk* mChunk;
};


TEST_P(MemoryChunkT, testConstructor) {
    const ChunkConfig& cfg = GetParam();

    // User-visible state of a MemoryChunk with default follow-up chunk ptr
    EXPECT_EQ(cfg.mNumBlocks, mChunk->availableBlocks());
    EXPECT_TRUE(0 == mChunk->getNextChunk());

    // User-visible state of a MemoryChunk with provided follow-up chunk ptr
    MemoryChunk* newChunk = new MemoryChunk(cfg.mChunkSize,
                                            cfg.mBlockSize,
                                            cfg.mNumBlocks,
                                            mChunk);
    EXPECT_EQ(cfg.mNumBlocks, newChunk->availableBlocks());
    EXPECT_TRUE(mChunk == newChunk->getNextChunk());

    // Cleanup
    delete newChunk;
}


TEST_P(MemoryChunkT, testAllocDealloc) {
    const ChunkConfig& cfg = GetParam();

    // Allocate all available blocks of the chunk, verifying that
    //  - returned pointers are non-NULL
    //  - number of available blocks is decreasing accordingly
    //  - offset between subsequent pointers is blockSize bytes
    vector<void*> alloc;
    alloc.reserve(cfg.mNumBlocks);
    for (size_t count = 0; count < cfg.mNumBlocks; ++count) {
        void* ptr = mChunk->allocate(cfg.mBlockSize);

        EXPECT_TRUE(0 != ptr);
        EXPECT_EQ(cfg.mNumBlocks - 1 - count, mChunk->availableBlocks());

        if (!alloc.empty()) {
            unsigned char* block = static_cast<unsigned char*>(ptr);
            unsigned char* last  = static_cast<unsigned char*>(alloc.back());

            ptrdiff_t offset = block - last;
            EXPECT_EQ(cfg.mBlockSize, offset);
        }

        alloc.push_back(ptr);
    }

    // Subsequent allocations should return nullptr
    void* ptr = mChunk->allocate(cfg.mBlockSize);
    EXPECT_TRUE(0 == ptr);

    // Deallocate blocks in non-consecutive order, with start index 6 and
    // stride 13.  Verify that number of available blocks is increasing
    // accordingly.
    vector<void*> dealloc;
    dealloc.reserve(cfg.mNumBlocks);
    size_t index = (6 % cfg.mNumBlocks);
    for (size_t count = 0; count < cfg.mNumBlocks; ++count) {
        void* ptr = alloc[index];

        mChunk->deallocate(ptr, cfg.mBlockSize);
        EXPECT_EQ(count + 1, mChunk->availableBlocks());

        index = (index + 13) % cfg.mNumBlocks;
        dealloc.push_back(ptr);
    }

    // Allocate blocks again; pointers should be in reverse deallocation order
    for (size_t count = 0; count < cfg.mNumBlocks; ++count) {
        void* ptr = mChunk->allocate(cfg.mBlockSize);

        EXPECT_EQ(dealloc.back(), ptr);
        dealloc.pop_back();
    }

    // Cleanup
    for (size_t count = 0; count < cfg.mNumBlocks; ++count) {
        mChunk->deallocate(alloc.back(), cfg.mBlockSize);
        alloc.pop_back();
    }
}


TEST_P(MemoryChunkT, testContains) {
    const ChunkConfig& cfg = GetParam();

    // Allocate all available blocks of the chunk; verify that blocks are
    // reported as being contained in the chunk
    vector<void*> alloc;
    alloc.reserve(cfg.mNumBlocks);
    for (size_t count = 0; count < cfg.mNumBlocks; ++count) {
        void* ptr = mChunk->allocate(cfg.mBlockSize);

        EXPECT_TRUE(mChunk->contains(ptr, cfg.mChunkSize));

        alloc.push_back(ptr);
    }

    // Allocate blocks of another chunk and verify that they are not reported
    // as being contained in the first chunk
    vector<void*> newAlloc;
    newAlloc.reserve(cfg.mNumBlocks);
    MemoryChunk* newChunk = new MemoryChunk(cfg.mChunkSize,
                                            cfg.mBlockSize,
                                            cfg.mNumBlocks);
    for (size_t count = 0; count < cfg.mNumBlocks; ++count) {
        void* ptr = newChunk->allocate(cfg.mBlockSize);

        EXPECT_FALSE(mChunk->contains(ptr, cfg.mChunkSize));

        newAlloc.push_back(ptr);
    }

    // Cleanup
    for (size_t count = 0; count < cfg.mNumBlocks; ++count) {
        mChunk->deallocate(alloc.back(), cfg.mBlockSize);
        alloc.pop_back();
        newChunk->deallocate(newAlloc.back(), cfg.mBlockSize);
        newAlloc.pop_back();
    }
    delete newChunk;
}


TEST_P(MemoryChunkT, testNextChunk) {
    const ChunkConfig& cfg = GetParam();

    MemoryChunk* newChunk = new MemoryChunk(cfg.mChunkSize,
                                            cfg.mBlockSize,
                                            cfg.mNumBlocks);
    EXPECT_TRUE(0 == mChunk->getNextChunk());

    // Verify that updated next chunk matches provided pointer
    mChunk->setNextChunk(newChunk);
    EXPECT_TRUE(newChunk == mChunk->getNextChunk());
    mChunk->setNextChunk(0);
    EXPECT_TRUE(0 == mChunk->getNextChunk());

    // Cleanup
    delete newChunk;
}


// Instantiate tests with various configurations
INSTANTIATE_TEST_CASE_P(MemoryChunk,
                        MemoryChunkT,
                        Values(

                            // Minimal block size, #blocks = max
                            ChunkConfig(256, 1),

                            // Small blocks, #blocks = max
                            ChunkConfig(1024, 4),

                            // Larger blocks, #blocks < max
                            ChunkConfig(4096, 32),

                            // Odd chunk/block size, #block < max,
                            // chunk memory not fully used
                            ChunkConfig(4023, 17),

                            // block size = chunk size, #blocks = 1
                            ChunkConfig(1024, 1024)
                        )
);
