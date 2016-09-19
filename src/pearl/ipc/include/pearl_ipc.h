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
 *  @brief   Declarations of inter-process communication functions.
 *
 *  This header file provides the declarations of internal PEARL library
 *  functions providing a basic inter-process communication layer. This
 *  layer abstracts from the details of different multi-process programming
 *  schemes and allows for client code to be written in a generic way.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_IPC_H
#define PEARL_IPC_H


#include <stdint.h>


namespace pearl
{
namespace detail
{
//--- Type definitions ------------------------------------------------------

/// @brief Datatypes used for inter-processes communication.
///
/// The %IpcDatatype enumeration defines possible values for the datatype
/// argument of communication operations. The defined symbolic names directly
/// correspond to the respective C++ datatypes.
///
/// @warning
///     If you modify this enumeration, remember to also adjust corresponding
///     lookup tables in all implementations of the communication layer.
///
enum IpcDatatype
{
    PEARL_INT8_T,     ///< 8-bit signed integer
    PEARL_UINT8_T,    ///< 8-bit unsigned integer
    PEARL_INT16_T,    ///< 16-bit signed integer
    PEARL_UINT16_T,   ///< 16-bit unsigned integer
    PEARL_INT32_T,    ///< 32-bit signed integer
    PEARL_UINT32_T,   ///< 32-bit unsigned integer
    PEARL_INT64_T,    ///< 64-bit signed integer
    PEARL_UINT64_T,   ///< 64-bit unsigned integer
    PEARL_FLOAT,      ///< 32-bit IEEE 754 floating-point value
    PEARL_DOUBLE      ///< 64-bit IEEE 754 floating-point value
};


//--- Function prototypes ---------------------------------------------------

/// @name IPC layer: Environmental management
/// @{

/// @brief Initialize PEARL's inter-process communication layer.
///
/// Initializes the internal inter-process communication layer. This routine
/// must be called before any other function of the communication layer, but
/// after the corresponding communication library (e.g., MPI) has been
/// initialized.
///
void
ipcInit();

/// @brief Finalize PEARL's inter-process communication layer.
///
/// Cleans up the internal inter-process communication layer. It must be
/// called before the corresponding communication library (e.g., MPI) has
/// been finalized.
///
void
ipcFinalize();

/// @brief Get total number of processes.
///
/// Returns the total number of processes for the current execution of the
/// application.
///
/// @returns
///     Total number of processes
///
uint32_t
ipcGetSize();

/// @brief Get rank of calling process.
///
/// Returns the "rank" of the calling process for the current execution of the
/// application, where "rank" is a globally unique integer ID in the range
/// [0,@#processes-1].
///
/// @returns
///     Rank of the calling process
///
uint32_t
ipcGetRank();

/// @}
/// @name IPC layer: Collective communication
/// @{

/// @brief Synchronize all processes.
///
/// This call synchronizes all processes. It returns at any process only after
/// all processes have entered the call.
///
void
ipcBarrier();

/// @brief Broadcast a message.
///
/// Broadcasts a message from the process with rank @a root to all processes,
/// itself included. On return, the content of @a root's @a buffer (i.e., a
/// message of @a count elements of the given @a datatype) is copied to all
/// other processes. All processes have to provide the same arguments for
/// @a count, @a datatype and @a root.
///
/// @param buffer
///     Pointer to send/receive buffer of size @a count * sizeof(@a datatype)
/// @param count
///     Number of data elements
/// @param datatype
///     Type of the data elements
/// @param root
///     Rank of broadcast root
///
void
ipcBroadcast(void*       buffer,
             uint32_t    count,
             IpcDatatype datatype,
             uint32_t    root);

/// @brief Gather fixed-sized data on one process.
///
/// Each process (root process included) sends a message of @a count elements
/// of the given @a datatype provided in @a sendBuffer to the process with rank
/// @a root. The root process receives the messages and stores them in rank
/// order in @a recvBuffer. All processes have to provide the same arguments
/// for @a count, @a datatype and @a root; the @a recvBuffer argument is only
/// significant at the root process.
///
/// @param sendBuffer
///     Pointer to send buffer of size @a count * sizeof(@a datatype)
/// @param recvBuffer
///     Pointer to receive buffer of size @#ranks * @a count *
///     sizeof(@a datatype) (significant only at root)
/// @param count
///     Number of data elements in send buffer
/// @param datatype
///     Type of the data elements
/// @param root
///     Rank of receiving process
///
void
ipcGather(const void* sendBuffer,
          void*       recvBuffer,
          uint32_t    count,
          IpcDatatype datatype,
          uint32_t    root);

/// @brief Gather variable-sized data on one process.
///
/// Each process (root process included) sends a message of @a sendCount
/// elements of the given @a datatype provided in @a sendBuffer to the process
/// with rank @a root. The root process receives the messages and stores them
/// in rank order in @a recvBuffer. All processes have to provide the same
/// arguments for @a datatype and @a root, however, processes can provide
/// different values for @a sendCount, with @a sendCount of process <i>i</i>
/// matching @a recvCounts[i]. The @a recvBuffer and @a recvCounts arguments
/// are only significant at the root process.
///
/// @param sendBuffer
///     Pointer to send buffer of size @a sendCount * sizeof(@a datatype)
/// @param sendCount
///     Number of data elements in send buffer
/// @param recvBuffer
///     Pointer to receive buffer of size \f$\sum_{i=0}^{\#ranks-1}\f$
///     recvCounts[i] * sizeof(@a datatype) (significant only at root)
/// @param recvCounts
///     Array containing the number of data elements received from each process
///     (significant only at root)
/// @param datatype
///     Type of the data elements
/// @param root
///     Rank of receiving process
///
void
ipcGatherv(const void*     sendBuffer,
           uint32_t        sendCount,
           void*           recvBuffer,
           const uint32_t* recvCounts,
           IpcDatatype     datatype,
           uint32_t        root);

/// @brief Scatter fixed-sized data to all processes.
///
/// Sends a message of @a count elements of the given @a datatype from the
/// process with rank @a root to all other processes. The message provided in
/// @a sendBuffer is split into <i>\#ranks</i> equal segments, the <i>i</i>-th
/// segment is sent to the <i>i</i>-th process. All processes have to provide
/// the same arguments for @a count, @a datatype and @a root; the @a sendBuffer
/// argument is only significant at the root process.
///
/// @param sendBuffer
///     Pointer to send buffer of size @#ranks * @a count * sizeof(@a datatype)
///     (significant only at root)
/// @param recvBuffer
///     Pointer to receive buffer of size @a count * sizeof(@a datatype)
/// @param count
///     Number of data elements to be sent to each process
/// @param datatype
///     Type of the data elements
/// @param root
///     Rank of sending process
///
void
ipcScatter(const void* sendBuffer,
           void*       recvBuffer,
           uint32_t    count,
           IpcDatatype datatype,
           uint32_t    root);

/// @brief Scatter variable-sized data to all processes.
///
/// Sends a message from the process with rank @a root to each other process.
/// The message provided in @a sendBuffer is split into <i>\#ranks</i> segments,
/// with the <i>i</i>-th segment of size @a sendCounts[i] * sizeof(datatype)
/// being sent to the <i>i</i>-th process. All processes have to provide the
/// same arguments for @a datatype and @a root, however, processes can provide
/// different values for @a recvCount, with @a recvCount of process <i>i</i>
/// matching @a sendCounts[i]. The @a sendBuffer and @a sendCounts arguments
/// are only significant at the root process.
///
/// @param sendBuffer
///     Pointer to send buffer of size \f$\sum_{i=0}^{\#ranks-1}\f$
///     sendCounts[i] * sizeof(@a datatype) (significant only at root)
/// @param sendCounts
///     Array containing the number of data elements sent to each process
///     (significant only at root)
/// @param recvBuffer
///     Pointer to receive buffer of size @a recvCount * sizeof(@a datatype)
/// @param recvCount
///     Number of data elements to be received
/// @param datatype
///     Type of the data elements
/// @param root
///     Rank of sending process
///
void
ipcScatterv(const void*     sendBuffer,
            const uint32_t* sendCounts,
            void*           recvBuffer,
            uint32_t        recvCount,
            IpcDatatype     datatype,
            uint32_t        root);

/// @}
}   // namespace detail
}   // namespace pearl


#endif   // !PEARL_IPC_H
