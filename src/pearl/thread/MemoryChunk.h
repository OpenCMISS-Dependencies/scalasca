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
 *  @brief   Declaration of the class pearl::detail::MemoryChunk.
 *
 *  This header file provides the declaration of the internal helper class
 *  pearl::detail::MemoryChunk.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_MEMORYCHUNK_H
#define PEARL_MEMORYCHUNK_H


#include <cassert>
#include <cstddef>

#include "wrap-memcheck.h"


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MemoryChunk
 *  @ingroup PEARL_thread
 *  @brief   Manages a chunk of memory consisting of an integral number of
 *           fixed-sized memory blocks.
 *
 *  Instances of the class %MemoryChunk form the lowest layer of PEARL's
 *  @ref PEARL_memory "custom memory management for small objects".  See
 *  the linked page for a motivation and a high-level design overview.
 *
 *  A %MemoryChunk instance manages a contiguous chunk of memory on the free
 *  store, which consists of an integral number of fixed-sized memory blocks
 *  that can be allocated and deallocated with low overhead.  Basically, a
 *  memory chunk can be considered as an array of fixed-sized memory blocks.
 *  Unallocated blocks within a chunk are managed using a free list, where
 *  the first byte of an unused block stores the array index of the next
 *  free block.  Using indices rather than pointers to build up the
 *  single-linked list of free blocks allows for block sizes smaller than
 *  `sizeof(void*)` and also avoids alignment issues (as bytes are always
 *  aligned).  As a downside, it limits the maximum number of blocks that
 *  can be managed by a single chunk to 256 blocks.
 *
 *  Each %MemoryChunk also keeps a pointer to another %MemoryChunk instance,
 *  which allows to organize chunks (typically of the same size) in a
 *  single-linked list.
 *
 *  @attention
 *  %MemoryChunk's are intended to be used by a higher-level allocator (see
 *  pearl::detail::ChunkAllocator), which already keeps track of basic
 *  parameters such as chunk and block sizes.  Therefore, this data is not
 *  redundantly stored in each %MemoryChunk instance, but has to be passed
 *  down where required.  Unfortunately, this limits the amount of sanity
 *  checking that can be done by %MemoryChunk's member functions.  **It is
 *  therefore the callers responsibility to ensure that the values passed
 *  to consecutive calls are consistent!**
 *
 *  @note
 *  Defining the preprocessor macro `HAVE_VALGRIND_MEMCHECK_H` to `1` during
 *  compilation will enable instrumentation for Valgrind's memcheck tool.
 **/
/*-------------------------------------------------------------------------*/

class MemoryChunk
{
    public:
        //--- Public member functions ----------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.  Allocates and initializes a chunk of memory.
        ///
        /// Allocates a contiguous chunk of memory of @a chunkSize bytes on
        /// the free store, capable of providing @a numBlocks fixed-sized
        /// memory blocks of @a blockSize bytes each.  Optionally, a pointer
        /// to a follow-up memory chunk in the chunk list can be provided as
        /// @a nextChunk (with the default being `nullptr`).  It is the
        /// callers responsibility to ensure that the provided numbers match
        /// and fulfill the required preconditions (see below).
        ///
        /// @param chunkSize
        ///     Size of the entire memory chunk in bytes
        /// @param blockSize
        ///     Size of each individual memory block in bytes
        /// @param numBlocks
        ///     Total number of memory blocks in the chunk
        /// @param nextChunk
        ///     Pointer to follow-up chunk
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate the memory chunk
        ///
        /// @pre
        ///     0 &lt; blockSize &le; chunkSize
        /// @pre
        ///     numBlocks = ( chunkSize / blockSize ) &le; 256
        ///
        MemoryChunk(std::size_t  chunkSize,
                    std::size_t  blockSize,
                    std::size_t  numBlocks,
                    MemoryChunk* nextChunk=0);

        /// @brief Destructor.
        ///
        /// Deallocates the associated chunk of memory and destroys the
        /// instance.  Note that a follow-up chunk is not deleted implicitly.
        ///
        ~MemoryChunk();

        /// @}
        /// @name Memory allocation routines
        /// @{

        /// @brief Allocate a single memory block.
        ///
        /// Allocates a single memory block of @a blockSize bytes from the
        /// chunk and returns a pointer to it.  If the request can not be
        /// fulfilled (i.e., if all blocks of the chunk are already occupied),
        /// `nullptr` is returned.
        ///
        /// @param blockSize
        ///     Size of the requested memory block in bytes
        /// @returns
        ///     Pointer to the memory block if successful; `nullptr` otherwise
        ///
        /// @pre
        ///     blockSize = block size specified at construction time
        ///
        void*
        allocate(std::size_t blockSize);

        /// @brief Deallocate a single memory block.
        ///
        /// Deallocates the memory block of @a blockSize bytes pointed to by
        /// @a ptr, which must have been returned by a previous call to
        /// allocate() of the same %MemoryChunk instance.
        ///
        /// @param ptr
        ///     Pointer to the memory block to deallocate
        /// @param blockSize
        ///     Size of the memory block in bytes
        ///
        /// @pre
        ///     ptr &isin; chunk ; ptr % blockSize = 0
        /// @pre
        ///     blockSize = block size specified at construction time
        ///
        void
        deallocate(void*       ptr,
                   std::size_t blockSize);

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Determine number of available blocks.
        ///
        /// Returns the total number of unallocated blocks in this chunk.
        ///
        /// @returns
        ///     Number of available blocks
        ///
        std::size_t
        availableBlocks() const;

        /// @brief Determine whether a block is contained in the chunk.
        ///
        /// Returns @e true if the memory block pointed to by @a ptr is
        /// contained in this chunk of @a chunkSize bytes.  Otherwise, it
        /// returns @e false.
        ///
        /// @warning
        /// This implementation relies on a strict total ordering of pointers,
        /// which is usually given on platforms using a continuous virtual
        /// address space.  In general, however, the result of comparing
        /// pointers of random origin is unspecified (see @cite ISOcpp:1998,
        /// &sect; 5.9, clause 2).
        ///
        /// @param ptr
        ///     Pointer to a memory block
        /// @param chunkSize
        ///     Size of the entire chunk in bytes
        /// @returns
        ///     True if the chunk contains @a block; false otherwise
        ///
        /// @pre
        ///     chunkSize = chunk size specified at construction time
        ///
        bool
        contains(void*       ptr,
                 std::size_t chunkSize) const;

        /// @}
        /// @name Chunk list handling
        /// @{

        /// @brief Determine follow-up chunk.
        ///
        /// Returns the pointer to the following memory chunk in the chunk
        /// list, or `nullptr` if the chunk constitutes the final element
        /// of the list.
        ///
        /// @returns
        ///     Pointer to follow-up chunk; `nullptr` if none exists
        ///
        MemoryChunk*
        getNextChunk() const;

        /// @brief Set follow-up chunk.
        ///
        /// Sets @a nextChunk as the pointer to the following memory chunk
        /// in the chunk list.  To mark the chunk as the final element of
        /// the list, `nullptr` should be passed.
        ///
        /// @param nextChunk
        ///     Pointer to follow-up chunk; `nullptr` if none exists
        ///
        void
        setNextChunk(MemoryChunk* nextChunk);

        /// @}


    private:
        //--- Data members ---------------------------------

        /// Pointer to the next %MemoryChunk instance
        MemoryChunk* mNextChunk;

        /// Pointer to the chunk's memory
        unsigned char* mData;

        /// Number of unallocated blocks
        unsigned short mAvailableBlocks;

        /// Index of first unallocated block
        unsigned char mFirstAvailableBlock;


        //--- Private member functions ---------------------

        // Make instances uncopyable
        MemoryChunk(const MemoryChunk& rhs);
        MemoryChunk&
        operator=(const MemoryChunk& rhs);
};


//--- Inline methods --------------------------------------------------------

#include "MemoryChunk-inl.h"
}   // namespace detail
}   // namespace pearl


#endif   // !PEARL_MEMORYCHUNK_H
