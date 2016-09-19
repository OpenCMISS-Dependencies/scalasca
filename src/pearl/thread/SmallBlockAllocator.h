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
 *  @brief   Declaration of the class pearl::detail::SmallBlockAllocator.
 *
 *  This header file provides the declaration of the internal helper class
 *  SmallBlockAllocator.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_SMALLBLOCKALLOCATOR_H
#define PEARL_SMALLBLOCKALLOCATOR_H


#include <cstddef>


namespace pearl
{
namespace detail
{
//--- Forward declarations --------------------------------------------------

class ChunkAllocator;


/*-------------------------------------------------------------------------*/
/**
 *  @class   SmallBlockAllocator
 *  @ingroup PEARL_thread
 *  @brief   Allocator for variable-sized small memory blocks.
 *
 *  The class %SmallBlockAllocator is part of PEARL's @ref PEARL_memory
 *  "custom memory management for small objects".  See the linked page for
 *  a motivation and a high-level design overview.
 *
 *  A %SmallBlockAllocator instance manages a (theoretically) unbounded
 *  number of variable-sized memory blocks on the free store, which can be
 *  allocated and deallocated with low overhead.  It handles allocation
 *  requests of up to 64 bytes using a set of pearl::detail::ChunkAllocator
 *  instances, while larger allocation requests are transparently forwarded
 *  to the default memory allocator `operator new`.  To reduce the number
 *  of (potentially) required %ChunkAllocator instances, a minimum alignment
 *  of 4 bytes is enforced.
 **/
/*-------------------------------------------------------------------------*/

class SmallBlockAllocator
{
    public:
        //--- Public member functions ----------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructs an allocator for variable-sized memory blocks.
        ///
        /// Creates a new allocator which is capable of managing allocations
        /// of variable-sized memory blocks.
        ///
        SmallBlockAllocator();

        /// @brief Destructor.
        ///
        /// Deallocates all memory chunks associated to the allocator and
        /// destroys the instance.
        ///
        ~SmallBlockAllocator();

        /// @}
        /// @name Memory allocation routines
        /// @{

        /// @brief Allocate a single memory block.
        ///
        /// Allocates a single memory block of @a blockSize bytes and returns
        /// a pointer to it.
        ///
        /// @param blockSize
        ///     Size of the requested memory block in bytes
        /// @returns
        ///     Pointer to the memory block
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate the memory block or any of the
        ///     internally required data structures
        ///
        void*
        allocate(std::size_t blockSize);

        /// @brief Deallocate a single memory block.
        ///
        /// Deallocates the memory block of @a blockSize bytes pointed to by
        /// @a ptr, which must have been returned by a previous call to
        /// allocate() of the same %SmallBlockAllocator instance.
        ///
        /// @param ptr
        ///     Pointer to the memory block to deallocate
        /// @param blockSize
        ///     Size of the memory block in bytes
        ///
        void
        deallocate(void*       ptr,
                   std::size_t blockSize);

        /// @}


    private:
        //--- Data members ---------------------------------

        /// Array of underlying fixed-sized memory allocators
        ChunkAllocator** mAllocators;


        //--- Private member functions ---------------------

        // Make instances uncopyable
        SmallBlockAllocator(const SmallBlockAllocator& rhs);
        SmallBlockAllocator&
        operator=(const SmallBlockAllocator& rhs);
};
}   // namespace detail
}   // namespace pearl


#endif   // !PEARL_SMALLBLOCKALLOCATOR_H
