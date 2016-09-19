/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2016                                                **
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
 *  @brief   OpenMP-specific mplementation of custom memory management
 *           functions.
 *
 *  This header file provides the OpenMP-specific implementation of the
 *  PEARL-internal custom memory management functions.  See pearl_memory.h
 *  for a detailed description of the API.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "pearl_memory.h"

#include <cassert>
#include <vector>

#include "SmallBlockAllocator.h"

using namespace std;
using namespace pearl::detail;


//--- Internal declarations -------------------------------------------------

namespace
{
/// Thread-local allocator used by the memory management
#ifdef __FUJITSU

// Due to issues with threadprivate OpenMP variables on K/FX10, we use the
// non-standard '__thread' storage class keyword (GNU extension, requires
// '-Xg' compiler flag) on this platform.
__thread SmallBlockAllocator* tls_allocator = 0;

#else   // !__FUJITSU

SmallBlockAllocator* tls_allocator = 0;
#pragma omp threadprivate(tls_allocator)

#endif   // !__FUJITSU

/// Container keeping track of all thread-local allocators
vector<SmallBlockAllocator*> allocators;
}   // unnamed namespace


//--- Memory management: Environmental management ---------------------------

void
pearl::detail::memoryFinalize()
{
    #pragma omp critical
    {
        vector<SmallBlockAllocator*>::iterator it = allocators.begin();
        while (it != allocators.end()) {
            delete *it;
            ++it;
        }
        allocators.clear();
    }
}


//--- Memory management: Allocation routines --------------------------------

void*
pearl::detail::memoryAllocate(const std::size_t blockSize)
{
    if (tls_allocator == 0) {
        tls_allocator = new SmallBlockAllocator;
        #pragma omp critical
        {
            allocators.push_back(tls_allocator);
        }
    }

    return tls_allocator->allocate(blockSize);
}


void
pearl::detail::memoryDeallocate(void* const       ptr,
                                const std::size_t blockSize)
{
    assert(tls_allocator);

    tls_allocator->deallocate(ptr, blockSize);
}
