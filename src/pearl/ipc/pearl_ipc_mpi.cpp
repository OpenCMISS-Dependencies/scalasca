/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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
 *  @ingroup PEARL_ipc
 *  @brief   Implementation of inter-process communication functions for
 *           MPI codes.
 *
 *  This file provides an implementation of the PEARL-internal inter-process
 *  communication functions using the Message Passing Interface (MPI) standard.
 *  See pearl_ipc.h for a detailed description of the API.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "pearl_ipc.h"

#include <cassert>
#include <climits>
#include <vector>

#include <mpi.h>

using namespace std;
using namespace pearl::detail;


//--- Local variables & helper functions ------------------------------------

namespace
{
/// MPI communicator used for internal communication
MPI_Comm ipcCommunicator = MPI_COMM_NULL;


/// @brief Map PEARL IPC datatype to MPI datatype
///
/// Returns the MPI datatype corresponding to the given PEARL IPC @a datatype.
///
/// @param ipcDatatype
///     PEARL IPC datatype
/// @returns
///     Corresponding MPI datatype
///
inline MPI_Datatype
getMpiDatatype(const IpcDatatype ipcDatatype)
{
    static const MPI_Datatype mpiDatatypes[] = {
        SCALASCA_MPI_INT8_T,
        SCALASCA_MPI_UINT8_T,
        SCALASCA_MPI_INT16_T,
        SCALASCA_MPI_UINT16_T,
        SCALASCA_MPI_INT32_T,
        SCALASCA_MPI_UINT32_T,
        SCALASCA_MPI_INT64_T,
        SCALASCA_MPI_UINT64_T,
        MPI_FLOAT,
        MPI_DOUBLE
    };
    return mpiDatatypes[ipcDatatype];
}


/// @brief Create a displacement vector
///
/// Stores the displacement of the data corresponding to process <i>i</i>
/// relative to the beginning of the send/receive buffer at the <i>i</i>-th
/// entry of the vector @a displs. The calculation is based on the element
/// counts provided in @a counts. The number of elements (i.e., the number
/// of processes) is given by @a numElements.
///
/// @param displs
///     Displacement vector
/// @param counts
///     Array containing the number of data elements for each process
/// @param numElements
///     Number of elements in @a counts
///
inline void
createDisplacements(vector<int>&          displs,
                    const uint32_t* const counts,
                    const uint32_t        numElements)
{
    displs.clear();
    displs.reserve(numElements);
    int offset = 0;
    for (uint32_t i = 0; i < numElements; ++i) {
        const uint32_t chunkSize = counts[i];

        displs.push_back(offset);
        offset += chunkSize;
    }
}


/// @brief Get pointer to vector data
///
/// Returns the pointer to the first data element of the vector @a array, or
/// NULL if the vector is empty.
///
/// @param array
///     Input vector
/// @returns
///     Pointer to first data element or NULL if vector is empty
///
inline int*
getDataPtr(vector<int>& array)
{
    if (array.empty()) {
        return 0;
    }
    return &array.front();
}


#if (SIZEOF_INT != SIZEOF_UINT32_T)
/// @brief Create a counts vector
///
/// Copies the element count data from @a inCounts to @a outCounts. The number
/// of elements (i.e., the number of processes) is given by @a numElements.
///
/// @note
///     Use of this function is only necessary if the datatypes @c int and
///     @c uint32_t have different sizes. Otherwise, a static_cast can be
///     used.
///
/// @param outCounts
///     Counts vector
/// @param inCounts
///     Array containing the number of data elements for each process
/// @param numElements
///     Number of elements in @a counts
///
inline void
copyCounts(vector<int>&          outCounts,
           const uint32_t* const inCounts,
           const uint32_t        numElements)
{
    outCounts.clear();
    outCounts.reserve(numElements);
    for (uint32_t i = 0; i < numElements; ++i) {
        outCounts.push_back(inCounts[i]);
    }
}
#endif
}   // unnamed namespace


//--- IPC layer: Environmental management -----------------------------------

void
pearl::detail::ipcInit()
{
    assert(ipcCommunicator == MPI_COMM_NULL);

    // Create internal resources
    MPI_Comm_dup(MPI_COMM_WORLD, &ipcCommunicator);
}


void
pearl::detail::ipcFinalize()
{
    assert(ipcCommunicator != MPI_COMM_NULL);

    // Release internal resources
    MPI_Comm_free(&ipcCommunicator);
}


uint32_t
pearl::detail::ipcGetSize()
{
    assert(ipcCommunicator != MPI_COMM_NULL);

    int size;
    MPI_Comm_size(ipcCommunicator, &size);

    return size;
}


uint32_t
pearl::detail::ipcGetRank()
{
    assert(ipcCommunicator != MPI_COMM_NULL);

    int rank;
    MPI_Comm_rank(ipcCommunicator, &rank);

    return rank;
}


//--- IPC layer: Collective communication -----------------------------------

void
pearl::detail::ipcBarrier()
{
    assert(ipcCommunicator != MPI_COMM_NULL);

    MPI_Barrier(ipcCommunicator);
}


void
pearl::detail::ipcBroadcast(void* const       buffer,
                            const uint32_t    count,
                            const IpcDatatype datatype,
                            const uint32_t    root)
{
    assert(ipcCommunicator != MPI_COMM_NULL);
    assert(buffer || (count == 0));
    assert(count <= INT_MAX);
    assert(root <= INT_MAX);

    MPI_Bcast(buffer, count, getMpiDatatype(datatype), root, ipcCommunicator);
}


/// @bug
///     <b>[MPI only]</b>
///     Casting away the @c const qualifier of the @a sendBuffer argument
///     may potentially cause trouble with MPI implementations that are
///     only compliant to a pre-3.0 version of the MPI standard, as these
///     made no guarantee that the buffer is not modified during the MPI
///     call.
///
void
pearl::detail::ipcGather(const void* const sendBuffer,
                         void* const       recvBuffer,
                         const uint32_t    count,
                         const IpcDatatype datatype,
                         const uint32_t    root)
{
    assert(ipcCommunicator != MPI_COMM_NULL);
    assert(sendBuffer || (count == 0));
    assert(recvBuffer || (ipcGetRank() != root));
    assert(count <= INT_MAX);
    assert(root <= INT_MAX);

#if (MPI_VERSION < 3)
    void* sendBufPtr = const_cast<void*>(sendBuffer);
#else
    const void* sendBufPtr = sendBuffer;
#endif

    const MPI_Datatype mpiDatatype = getMpiDatatype(datatype);
    MPI_Gather(sendBufPtr, count, mpiDatatype,
               recvBuffer, count, mpiDatatype,
               root, ipcCommunicator);
}


/// @bug
///     <b>[MPI only]</b>
///     Casting away the @c const qualifier of the @a sendBuffer and
///     @a recvCounts arguments may potentially cause trouble with MPI
///     implementations that are only compliant to a pre-3.0 version of
///     the MPI standard, as these made no guarantee that the buffers
///     are not modified during the MPI call.
///
void
pearl::detail::ipcGatherv(const void* const     sendBuffer,
                          const uint32_t        sendCount,
                          void* const           recvBuffer,
                          const uint32_t* const recvCounts,
                          const IpcDatatype     datatype,
                          const uint32_t        root)
{
    assert(ipcCommunicator != MPI_COMM_NULL);
    assert(sendBuffer || (sendCount == 0));
    assert(sendCount <= INT_MAX);
    assert(root <= INT_MAX);

    const uint32_t numRanks = ipcGetSize();
    const uint32_t rank     = ipcGetRank();

    assert(recvBuffer || (rank != root));
    assert(recvCounts || (rank != root));

#if (SIZEOF_INT != SIZEOF_UINT32_T)
    // Setup receive counts
    vector<int> counts;
    if (rank == root) {
        copyCounts(counts, recvCounts, numRanks);
    }
    const int* recvCountsPtr = getDataPtr(counts);
#else
    const void* recvCountsTmp = recvCounts;
    const int*  recvCountsPtr = static_cast<const int*>(recvCountsTmp);
#endif

    // Setup displacements
    vector<int> displs;
    if (rank == root) {
        createDisplacements(displs, recvCounts, numRanks);
    }

#if (MPI_VERSION < 3)
    void* sendBufPtr = const_cast<void*>(sendBuffer);
    int*  countsPtr  = const_cast<int*>(recvCountsPtr);
#else
    const void* sendBufPtr = sendBuffer;
    const int*  countsPtr  = recvCountsPtr;
#endif

    const MPI_Datatype mpiDatatype = getMpiDatatype(datatype);
    MPI_Gatherv(sendBufPtr, sendCount, mpiDatatype,
                recvBuffer, countsPtr, getDataPtr(displs), mpiDatatype,
                root, ipcCommunicator);
}


/// @bug
///     <b>[MPI only]</b>
///     Casting away the @c const qualifier of the @a sendBuffer argument
///     may potentially cause trouble with MPI implementations that are
///     only compliant to a pre-3.0 version of the MPI standard, as these
///     made no guarantee that the buffer is not modified during the MPI
///     call.
///
void
pearl::detail::ipcScatter(const void* const sendBuffer,
                          void* const       recvBuffer,
                          const uint32_t    count,
                          const IpcDatatype datatype,
                          const uint32_t    root)
{
    assert(ipcCommunicator != MPI_COMM_NULL);
    assert(sendBuffer || (ipcGetRank() != root));
    assert(recvBuffer || count == 0);
    assert(count <= INT_MAX);
    assert(root <= INT_MAX);

#if (MPI_VERSION < 3)
    void* sendBufPtr = const_cast<void*>(sendBuffer);
#else
    const void* sendBufPtr = sendBuffer;
#endif

    const MPI_Datatype mpiDatatype = getMpiDatatype(datatype);
    MPI_Scatter(sendBufPtr, count, mpiDatatype,
                recvBuffer, count, mpiDatatype,
                root, ipcCommunicator);
}


/// @bug
///     <b>[MPI only]</b>
///     Casting away the @c const qualifier of the @a sendBuffer and
///     @a sendCounts arguments may potentially cause trouble with MPI
///     implementations that are only compliant to a pre-3.0 version of
///     the MPI standard, as these made no guarantee that the buffers
///     are not modified during the MPI call.
///
void
pearl::detail::ipcScatterv(const void* const     sendBuffer,
                           const uint32_t* const sendCounts,
                           void* const           recvBuffer,
                           const uint32_t        recvCount,
                           const IpcDatatype     datatype,
                           const uint32_t        root)
{
    assert(ipcCommunicator != MPI_COMM_NULL);
    assert(recvBuffer || recvCount == 0);
    assert(recvCount <= INT_MAX);
    assert(root <= INT_MAX);

    const uint32_t numRanks = ipcGetSize();
    const uint32_t rank     = ipcGetRank();

    assert(sendBuffer || (rank != root));
    assert(sendCounts || (rank != root));

#if (SIZEOF_INT != SIZEOF_UINT32_T)
    // Setup send counts
    vector<int> counts;
    if (rank == root) {
        copyCounts(counts, sendCounts, numRanks);
    }
    const int* sendCountsPtr = getDataPtr(counts);
#else
    const void* sendCountsTmp = sendCounts;
    const int*  sendCountsPtr = static_cast<const int*>(sendCountsTmp);
#endif

    // Setup displacements
    vector<int> displs;
    if (rank == root) {
        createDisplacements(displs, sendCounts, numRanks);
    }

#if (MPI_VERSION < 3)
    void* sendBufPtr = const_cast<void*>(sendBuffer);
    int*  countsPtr  = const_cast<int*>(sendCountsPtr);
#else
    const void* sendBufPtr = sendBuffer;
    const int*  countsPtr  = sendCountsPtr;
#endif

    const MPI_Datatype mpiDatatype = getMpiDatatype(datatype);
    MPI_Scatterv(sendBufPtr, countsPtr, getDataPtr(displs), mpiDatatype,
                 recvBuffer, recvCount, mpiDatatype,
                 root, ipcCommunicator);
}
