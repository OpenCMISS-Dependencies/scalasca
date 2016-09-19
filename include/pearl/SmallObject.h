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
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class pearl::SmallObject.
 *
 *  This header file provides the declaration of the class
 *  pearl::SmallObject.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_SMALLOBJECT_H
#define PEARL_SMALLOBJECT_H


#include <cstddef>


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   SmallObject
 *  @ingroup PEARL_base
 *  @brief   Provides a base class for small objects using a custom memory
 *           management.
 *
 *  Each dynamically allocated memory block usually requires some extra
 *  memory for bookkeeping purposes.  However, this can be quite space
 *  inefficient if a large number of small objects is allocated, as the
 *  per-object overhead is significant.
 *
 *  The SmallObject class serves as a base class that can be used for these
 *  kind of small objects, providing a customized memory management that
 *  allocates memory in larger chunks rather than individually for each
 *  object.  This specialized memory management is restricted to objects of
 *  not more than 64 bytes, however, if objects grow larger (e.g., objects
 *  of a derived class) the default memory allocation routines are used
 *  transparently.  Note that array allocations of small objects are always
 *  handled by the default `operator new` allocator.
 **/
/*-------------------------------------------------------------------------*/

class SmallObject
{
    public:
        //--- Public member functions ----------------------

        /// @name Memory allocation operators
        /// @{

        /// @brief Allocate storage space.
        ///
        /// Allocates @a objectSize bytes of storage space and returns a
        /// non-null pointer to the first byte of the block.  An optimized
        /// allocator is used for small objects less than 64 bytes, the
        /// default memory allocator otherwise.
        ///
        /// @param objectSize
        ///     Size of the requested memory block in bytes
        ///
        /// @throws bad_alloc
        ///     on failure to allocate the object memory or any of the
        ///     internally required data structures
        ///
        static void*
        operator new(std::size_t objectSize);

        /// @brief Deallocate storage space.
        ///
        /// Deallocates the memory block pointed to by @a deadObject if it
        /// is non-null, releasing memory previously allocated by a call to
        /// SmallObject::operator new. If @a deadObject is `nullptr`, this
        /// operator is a no-op.
        ///
        /// @param deadObject
        ///     Pointer to memory block to be released
        /// @param objectSize
        ///     Size of the memory block in bytes
        ///
        static void
        operator delete(void*       deadObject,
                        std::size_t objectSize);

        /// @}


    protected:
        //--- Protected member functions -------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Destructor.
        ///
        /// Releases all allocated resources and destroys the instance.
        ///
        ~SmallObject();

        /// @}
};
}   // namespace pearl


#endif   // !PEARL_SMALLOBJECT_H
