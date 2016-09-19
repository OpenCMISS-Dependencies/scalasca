/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include "Otf2CollCallbacks.h"
#include "Otf2TypeHelpers.h"

#include <cassert>
#include <exception>

#include <pearl_ipc.h>

using namespace std;
using namespace pearl::detail;


//--- OTF2: Collective callbacks --------------------------------------------

OTF2_CallbackCode
pearl::detail::otf2CollCbGetSize(void* const                   userData,
                                 OTF2_CollectiveContext* const commContext,
                                 uint32_t* const               size)
{
    assert(size);

    try {
        *size = ipcGetSize();
    }
    catch (exception& ex) {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbGetRank(void* const                   userData,
                                 OTF2_CollectiveContext* const commContext,
                                 uint32_t* const               rank)
{
    assert(rank);

    try {
        *rank = ipcGetRank();
    }
    catch (exception& ex) {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbBarrier(void* const                   userData,
                                 OTF2_CollectiveContext* const commContext)
{
    try {
        ipcBarrier();
    }
    catch (exception& ex) {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbBroadcast(void* const                   userData,
                                   OTF2_CollectiveContext* const commContext,
                                   void* const                   buffer,
                                   const uint32_t                count,
                                   const OTF2_Type               type,
                                   const uint32_t                root)
{
    try {
        ipcBroadcast(buffer, count, otf2GetIpcDatatype(type), root);
    }
    catch (exception& ex) {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbGather(void* const                   userData,
                                OTF2_CollectiveContext* const commContext,
                                const void* const             sendBuffer,
                                void* const                   recvBuffer,
                                const uint32_t                count,
                                const OTF2_Type               type,
                                const uint32_t                root)
{
    try {
        ipcGather(sendBuffer, recvBuffer, count, otf2GetIpcDatatype(type), root);
    }
    catch (exception& ex) {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbGatherv(void* const                   userData,
                                 OTF2_CollectiveContext* const commContext,
                                 const void* const             sendBuffer,
                                 const uint32_t                sendCount,
                                 void* const                   recvBuffer,
                                 const uint32_t* const         recvCounts,
                                 const OTF2_Type               type,
                                 const uint32_t                root)
{
    try {
        ipcGatherv(sendBuffer, sendCount,
                   recvBuffer, recvCounts, otf2GetIpcDatatype(type), root);
    }
    catch (exception& ex) {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbScatter(void* const                   userData,
                                 OTF2_CollectiveContext* const commContext,
                                 const void* const             sendBuffer,
                                 void* const                   recvBuffer,
                                 const uint32_t                count,
                                 const OTF2_Type               type,
                                 const uint32_t                root)
{
    try {
        ipcScatter(sendBuffer, recvBuffer, count, otf2GetIpcDatatype(type), root);
    }
    catch (exception& ex) {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2CollCbScatterv(void* const                   userData,
                                  OTF2_CollectiveContext* const commContext,
                                  const void* const             sendBuffer,
                                  const uint32_t* const         sendCounts,
                                  void* const                   recvBuffer,
                                  const uint32_t                recvCount,
                                  const OTF2_Type               type,
                                  const uint32_t                root)
{
    try {
        ipcScatterv(sendBuffer, sendCounts,
                    recvBuffer, recvCount, otf2GetIpcDatatype(type), root);
    }
    catch (exception& ex) {
        return OTF2_CALLBACK_ERROR;
    }

    return OTF2_CALLBACK_SUCCESS;
}
