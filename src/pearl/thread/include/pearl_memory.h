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
 *  @brief   Declarations of custom memory management functions.
 *
 *  This header file provides the declarations of custom memory management
 *  functions internally used by the PEARL library.  This layer abstracts
 *  from the different implementations for the supported multi-threaded
 *  programming paradigms and allows for client code to be written in a
 *  generic way.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_MEMORY_H
#define PEARL_MEMORY_H


#include <cstddef>


namespace pearl
{
namespace detail
{
//--- Function prototypes ---------------------------------------------------

/// @name Memory management: Environmental management
/// @{

/// @brief Initialize PEARL's custom memory management layer.
///
/// Initializes the PEARL-internal custom memory management and installs a
/// custom out-of-memory handler.  This routine must be called in a
/// single-threaded context before any PEARL memory management routine is
/// called.
///
void
memoryInit();

/// @brief Finalize PEARL's custom memory management layer.
///
/// Cleans up all PEARL-internal custom memory management state.  It must be
/// called in a single-threaded context.  Afterwards, no further PEARL memory
/// management routines may be called.
///
void
memoryFinalize();

/// @}
/// @name Memory management: Allocation routines
/// @{

/// @brief Allocate a single memory block.
///
/// Allocates a single memory block of @a blockSize bytes and returns a
/// pointer to it.
///
/// @param blockSize
///     Size of the requested memory block in bytes
/// @returns
///     Pointer to the memory block
///
/// @throws std::bad_alloc
///     on failure to allocate the memory block or any of the internally
///     required data structures
///
void*
memoryAllocate(std::size_t blockSize);

/// @brief Deallocate a single memory block.
///
/// Deallocates the memory block of @a blockSize bytes pointed to by @a ptr,
/// which must have been returned by a previous call to memoryAllocate().
///
/// @warning
/// Currently, deallocations of memory blocks must happen on the same thread
/// than the corresponding allocation! Otherwise, the application will abort.
///
/// @param ptr
///     Pointer to the memory block to deallocate
/// @param blockSize
///     Size of the memory block in bytes
///
void
memoryDeallocate(void*       ptr,
                 std::size_t blockSize);

/// @}
}   // namespace detail
}   // namespace pearl


#endif   // !PEARL_MEMORY_H
