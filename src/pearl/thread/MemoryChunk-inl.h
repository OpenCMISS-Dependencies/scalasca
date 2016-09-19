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


//--- Memory allocation routines (inline) -----------------------------------

inline void*
MemoryChunk::allocate(const std::size_t blockSize)
{
    assert(blockSize > 0);

    // All blocks occupied => nullptr
    if (mAvailableBlocks == 0) {
        return 0;
    }

    // Bookkeeping, tweaking Valgrind's accessibility flags to avoid
    // "invalid read" errors
    unsigned char* newBlock = mData + (mFirstAvailableBlock * blockSize);
    VALGRIND_MALLOCLIKE_BLOCK(newBlock, blockSize, 0, 0);
    VALGRIND_MAKE_MEM_DEFINED(newBlock, sizeof(unsigned char));
    mFirstAvailableBlock = *newBlock;
    VALGRIND_MAKE_MEM_UNDEFINED(newBlock, sizeof(unsigned char));
    --mAvailableBlocks;

    return newBlock;
}


inline void
MemoryChunk::deallocate(void* const       ptr,
                        const std::size_t blockSize)
{
    assert(ptr != 0);
    assert(blockSize > 0);

    unsigned char* const block = static_cast<unsigned char*>(ptr);
    assert(block >= mData);

    const std::ptrdiff_t offset = block - mData;
    assert(offset % blockSize == 0);
    assert(offset / blockSize < 256);

    // Bookkeeping, tweaking Valgrind's accessibility flags to avoid
    // "invalid write" errors
    VALGRIND_FREELIKE_BLOCK(block, 0);
    VALGRIND_MAKE_MEM_UNDEFINED(block, sizeof(unsigned char));
    *block = mFirstAvailableBlock;
    VALGRIND_MAKE_MEM_NOACCESS(block, sizeof(unsigned char));
    mFirstAvailableBlock = static_cast<unsigned char>(offset / blockSize);
    ++mAvailableBlocks;
}


//--- Query functions (inline) ----------------------------------------------

inline std::size_t
MemoryChunk::availableBlocks() const
{
    return mAvailableBlocks;
}


inline bool
MemoryChunk::contains(void* const       ptr,
                      const std::size_t chunkSize) const
{
    unsigned char* const block = static_cast<unsigned char*>(ptr);

    return (block >= mData) && (block < mData + chunkSize);
}


//--- Chunk list handling (inline) ------------------------------------------

inline MemoryChunk*
MemoryChunk::getNextChunk() const
{
    return mNextChunk;
}


inline void
MemoryChunk::setNextChunk(MemoryChunk* const nextChunk)
{
    mNextChunk = nextChunk;
}
