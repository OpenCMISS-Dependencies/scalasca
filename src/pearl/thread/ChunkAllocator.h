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
 *  @brief   Declaration of the class pearl::detail::ChunkAllocator.
 *
 *  This header file provides the declaration of the internal helper class
 *  pearl::detail::ChunkAllocator.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_CHUNKALLOCATOR_H
#define PEARL_CHUNKALLOCATOR_H


#include <cstddef>


namespace pearl
{
namespace detail
{
//--- Forward declarations --------------------------------------------------

class MemoryChunk;


/*-------------------------------------------------------------------------*/
/**
 *  @class   ChunkAllocator
 *  @ingroup PEARL_thread
 *  @brief   Allocator for fixed-sized memory blocks, allocating memory in
 *           in larger chunks.
 *
 *  The class %ChunkAllocator is part of PEARL's @ref PEARL_memory
 *  "custom memory management for small objects".  See the linked page for
 *  a motivation and a high-level design overview.
 *
 *  A %ChunkAllocator instance manages a (theoretically) unbounded number of
 *  fixed-sized memory blocks on the free store, which can be allocated and
 *  deallocated with low overhead.  It is particularly designed for small
 *  memory blocks, using pearl::detail::MemoryChunk as a basic block to
 *  allocate memory in larger chunks.  Since %MemoryChunk instances can only
 *  handle a fixed number of blocks, %ChunkAllocator organizes multiple
 *  chunks in a single-linked list.
 *
 *  To speed up common memory (de-)allocation patterns in the PEARL library,
 *  %ChunkAllocator uses multiple caches:
 *
 *   - The *allocation cache* either points to the last (initially empty)
 *     memory chunk added to the chunk list, or to the last chunk where an
 *     allocation or deallocation took place.  This way, many allocation
 *     requests can be fulfilled without searching the chunk list for a
 *     chunk with an available block.
 *   - The *deallocation cache* either points to the last chunk where
 *     a deallocation took place, or to its follow-up chunk if a now
 *     unused chunk was removed from the chunk list during deallocation.
 *     In many cases, this avoids searching the chunk list for the chunk
 *     containing the block pointer to be deallocated, assuming bulk
 *     deallocations in FIFO order.
 *   - The *empty chunk cache* points to (at most) a single, unused
 *     %MemoryChunk instance.  This helps to speed up borderline cases,
 *     where a deallocation freeing the last occupied block of a chunk
 *     is followed by an allocation.
 **/
/*-------------------------------------------------------------------------*/

class ChunkAllocator
{
    public:
        //--- Public member functions ----------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructs an allocator for fixed-sized memory blocks.
        ///
        /// Creates a new allocator which is capable of managing allocations
        /// of fixed-sized memory blocks of @a blockSize bytes.
        ///
        /// @param blockSize
        ///     Size of the managed memory blocks in bytes
        ///
        /// @pre
        ///     0 < blockSize &le; 4096
        ///
        explicit
        ChunkAllocator(std::size_t blockSize);

        /// @brief Destructor.
        ///
        /// Deallocates all memory chunks associated to the allocator and
        /// destroys the instance.
        ///
        ~ChunkAllocator();

        /// @}
        /// @name Memory allocation routines
        /// @{

        /// @brief Allocate a single memory block.
        ///
        /// Allocates a single memory block and returns a pointer to it.
        ///
        /// @returns
        ///     Pointer to the memory block
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate a new memory chunk
        ///
        void*
        allocate();

        /// @brief Deallocate a single memory block.
        ///
        /// Deallocates the memory block pointed to by @a ptr, which must
        /// have been returned by a previous call to allocate() of the same
        /// %ChunkAllocator instance.
        ///
        /// @param ptr
        ///     Pointer to the memory block to deallocate
        ///
        void
        deallocate(void* ptr);

        /// @}


    private:
        //--- Data members ---------------------------------

        /// Pointer to the head element of the chunk list
        MemoryChunk* mChunkListHead;

        /// Pointer to the tail element of the chunk list
        MemoryChunk* mChunkListTail;

        /// Empty chunk cache; points to at most one empty chunk for reuse
        MemoryChunk* mEmptyCache;

        /// Allocation cache; points to chunk of last (de-)allocation
        MemoryChunk* mAllocCache;

        /// Deallocation cache; points to chunk of last deallocation
        MemoryChunk* mDeallocCache;

        /// Pointer to predecessor chunk of deallocation cache
        MemoryChunk* mDeallocCachePrev;

        /// Size of the managed memory blocks in bytes
        std::size_t mBlockSize;

        /// Total number of unallocated blocks across all chunks
        std::size_t mAvailableBlocks;


        //--- Private member functions ---------------------

        // Make instances uncopyable
        ChunkAllocator(const ChunkAllocator& rhs);
        ChunkAllocator&
        operator=(const ChunkAllocator& rhs);
};
}   // namespace detail
}   // namespace pearl


#endif   // !PEARL_CHUNKALLOCATOR_H
