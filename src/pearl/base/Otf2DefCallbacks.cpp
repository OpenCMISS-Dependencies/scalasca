/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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
 *  @brief   Implementation of OTF2 callback functions for definition records.
 *
 *  This file provides the implementation of OTF2 callback functions for
 *  handling global definition records.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>
#include "Otf2DefCallbacks.h"
#include "Otf2TypeHelpers.h"

#include <cassert>
#include <exception>
#include <string>

#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>

#include "DefsFactory.h"
#include "Process.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


//--- Local helper macros ---------------------------------------------------

#define CALLBACK_SETUP                                            \
    Otf2DefCbData * data = static_cast<Otf2DefCbData*>(userData); \
    GlobalDefs& defs = data->mDefinitions;                        \
    try {
#define CALLBACK_CLEANUP                 \
    }     /* Closes the try block */     \
    catch (std::exception& ex) {         \
        data->mErrorMessage = ex.what(); \
        return OTF2_CALLBACK_INTERRUPT;  \
    }


//--- OTF2: Global definition callbacks -------------------------------------

/// @todo
///     Implement callback; Attribute definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbAttribute(void* const             userData,
                                  const OTF2_AttributeRef self,
                                  const OTF2_StringRef    name,
                                  const OTF2_StringRef    description,
                                  const OTF2_Type         type)
{
    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2DefCbCallingContext(void* const                      userData,
                                       const OTF2_CallingContextRef     self,
                                       const OTF2_RegionRef             region,
                                       const OTF2_SourceCodeLocationRef sourceCodeLocation,
                                       const OTF2_CallingContextRef     parent)
{
    CALLBACK_SETUP
    DefsFactory:: instance()->createCallingContext(defs,
                                                   self,
                                                   region,
                                                   sourceCodeLocation,
                                                   parent);
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; CallingContextProperty definitions are currently
///     ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbCallingContextProperty(void* const                  userData,
                                               const OTF2_CallingContextRef callingContext,
                                               const OTF2_StringRef         name,
                                               const OTF2_Type              type,
                                               const OTF2_AttributeValue    value)
{
    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2DefCbCallpath(void* const            userData,
                                 const OTF2_CallpathRef self,
                                 const OTF2_CallpathRef parent,
                                 const OTF2_RegionRef   region)
{
    CALLBACK_SETUP
    DefsFactory:: instance()->createCallpath(defs,
                                             self,
                                             region,
                                             Callsite::NO_ID,
                                             parent);
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Handle @a leftRegion
OTF2_CallbackCode
pearl::detail::otf2DefCbCallsite(void* const            userData,
                                 const OTF2_CallsiteRef self,
                                 const OTF2_StringRef   sourceFile,
                                 const uint32_t         lineNumber,
                                 const OTF2_RegionRef   enteredRegion,
                                 const OTF2_RegionRef   leftRegion)
{
    CALLBACK_SETUP
    DefsFactory:: instance()->createCallsite(defs,
                                             self,
                                             sourceFile,
                                             lineNumber,
                                             enteredRegion);
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; CartCoordinate definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbCartCoordinate(void* const                userData,
                                       const OTF2_CartTopologyRef cartTopology,
                                       const uint32_t             rank,
                                       const uint8_t              numberOfDimensions,
                                       const uint32_t* const      coordinates)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; CartDimension definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbCartDimension(void* const                 userData,
                                      const OTF2_CartDimensionRef self,
                                      const OTF2_StringRef        name,
                                      const uint32_t              size,
                                      const OTF2_CartPeriodicity  cartPeriodicity)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; CartTopology definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbCartTopology(void* const                        userData,
                                     const OTF2_CartTopologyRef         self,
                                     const OTF2_StringRef               name,
                                     const OTF2_CommRef                 communicator,
                                     const uint8_t                      numberOfDimensions,
                                     const OTF2_CartDimensionRef* const cartDimensions)
{
    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2DefCbClockProperties(void* const    userData,
                                        const uint64_t timerResolution,
                                        const uint64_t globalOffset,
                                        const uint64_t traceLength)
{
    CALLBACK_SETUP
    data-> mTimerResolution = timerResolution;
    data->mGlobalOffset = globalOffset;
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Handle all communicators once all groups are handled
OTF2_CallbackCode
pearl::detail::otf2DefCbComm(void* const          userData,
                             const OTF2_CommRef   self,
                             const OTF2_StringRef name,
                             const OTF2_GroupRef  group,
                             const OTF2_CommRef   parent)
{
    CALLBACK_SETUP

    // Create communicator if associated group exists (only MPI groups are
    // handled right now => only create MPI communicators)
    try {
        const ProcessGroup& processGroup = defs.getProcessGroup(group);
        DefsFactory::instance()->createMpiComm(defs,
                                               self,
                                               name,
                                               processGroup.getId(),
                                               parent);
    }
    catch (const RuntimeError& ex) {
        // Unfortunately, we can't distinguish between an invalid ID
        // and a group that was ignored during reading yet...
        return OTF2_CALLBACK_SUCCESS;
    }

    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Handle location, region and metric groups
/// @todo
///     Document which kind of definitions may be referenced my @a members
///     parameter
OTF2_CallbackCode
pearl::detail::otf2DefCbGroup(void* const           userData,
                              const OTF2_GroupRef   self,
                              const OTF2_StringRef  name,
                              const OTF2_GroupType  groupType,
                              const OTF2_Paradigm   paradigm,
                              const OTF2_GroupFlag  groupFlags,
                              const uint32_t        numberOfMembers,
                              const uint64_t* const members)
{
    CALLBACK_SETUP

    // Only handle MPI groups for now
    if (OTF2_PARADIGM_MPI != paradigm) {
        return OTF2_CALLBACK_SUCCESS;
    }

    // Case 1: MPI group
    if ((OTF2_GROUP_TYPE_COMM_GROUP == groupType)
        || (OTF2_GROUP_TYPE_COMM_SELF == groupType)) {
        bool isSelf = (OTF2_GROUP_TYPE_COMM_SELF == groupType);

        // Set up ranks array
        DefsFactory::process_group ranks;
        ranks.reserve(numberOfMembers);
        for (uint32_t index = 0; index < numberOfMembers; ++index) {
            ranks.push_back(members[index]);
        }

        // Create group
        DefsFactory::instance()->createMpiGroup(defs,
                                                self,
                                                name,
                                                ranks,
                                                isSelf,
                                                false);
    }

    // Case 2: MPI locations
    if (OTF2_GROUP_TYPE_COMM_LOCATIONS == groupType) {
        // Set rank of location groups
        for (uint32_t rank = 0; rank < numberOfMembers; ++rank) {
            uint64_t locationId = members[rank];

            const Location& location = defs.getLocation(locationId);
            Process*        process  = static_cast<Process*>(location.getParent());
            process->setRank(rank);
        }
    }
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; InterruptGenerator definitions are currently
///     ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbInterruptGenerator(void* const                       userData,
                                           const OTF2_InterruptGeneratorRef  self,
                                           const OTF2_StringRef              name,
                                           const OTF2_InterruptGeneratorMode mode,
                                           const OTF2_Base                   base,
                                           const int64_t                     exponent,
                                           const uint64_t                    period)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Handle non-CPU type locations
OTF2_CallbackCode
pearl::detail::otf2DefCbLocation(void* const                 userData,
                                 const OTF2_LocationRef      self,
                                 const OTF2_StringRef        name,
                                 const OTF2_LocationType     locationType,
                                 const uint64_t              numberOfEvents,
                                 const OTF2_LocationGroupRef locationGroup)
{
    CALLBACK_SETUP

    // Sanity check
    if (OTF2_LOCATION_TYPE_CPU_THREAD != locationType) {
        throw RuntimeError("Locations of non-CPU type not yet supported!");
    }
    DefsFactory::instance()->createLocation(defs,
                                            self,
                                            name,
                                            otf2ConvertLocationType(locationType),
                                            numberOfEvents,
                                            locationGroup);
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2DefCbLocationGroup(void* const                  userData,
                                      const OTF2_LocationGroupRef  self,
                                      const OTF2_StringRef         name,
                                      const OTF2_LocationGroupType locationGroupType,
                                      const OTF2_SystemTreeNodeRef systemTreeParent)
{
    CALLBACK_SETUP
    DefsFactory:: instance()->createLocationGroup(defs,
                                                  self,
                                                  name,
                                                  otf2ConvertLocationGroupType(locationGroupType),
                                                  systemTreeParent);
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; LocationGroupProperty definitions are currently
///     ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbLocationGroupProperty(void* const                 userData,
                                              const OTF2_LocationGroupRef locationGroup,
                                              const OTF2_StringRef        name,
                                              const OTF2_Type             type,
                                              const OTF2_AttributeValue   value)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; LocationProperty definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbLocationProperty(void* const               userData,
                                         const OTF2_LocationRef    location,
                                         const OTF2_StringRef      name,
                                         const OTF2_Type           type,
                                         const OTF2_AttributeValue value)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; MetricClass definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbMetricClass(void* const                       userData,
                                    const OTF2_MetricRef              self,
                                    const uint8_t                     numberOfMetrics,
                                    const OTF2_MetricMemberRef* const metricMembers,
                                    const OTF2_MetricOccurrence       metricOccurrence,
                                    const OTF2_RecorderKind           recorderKind)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; MetricClassRecorder definitions are currently
///     ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbMetricClassRecorder(void* const            userData,
                                            const OTF2_MetricRef   metricClass,
                                            const OTF2_LocationRef recorder)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; MetricInstance definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbMetricInstance(void* const            userData,
                                       const OTF2_MetricRef   self,
                                       const OTF2_MetricRef   metricClass,
                                       const OTF2_LocationRef recorder,
                                       const OTF2_MetricScope metricScope,
                                       const uint64_t         scope)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; MetricMember definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbMetricMember(void* const                userData,
                                     const OTF2_MetricMemberRef self,
                                     const OTF2_StringRef       name,
                                     const OTF2_StringRef       description,
                                     const OTF2_MetricType      metricType,
                                     const OTF2_MetricMode      metricMode,
                                     const OTF2_Type            valueType,
                                     const OTF2_Base            base,
                                     const int64_t              exponent,
                                     const OTF2_StringRef       unit)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; Paradigm definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbParadigm(void* const              userData,
                                 const OTF2_Paradigm      paradigm,
                                 const OTF2_StringRef     name,
                                 const OTF2_ParadigmClass paradigmClass)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; ParadigmProperty definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbParadigmProperty(void* const                 userData,
                                         const OTF2_Paradigm         paradigm,
                                         const OTF2_ParadigmProperty property,
                                         const OTF2_Type             type,
                                         const OTF2_AttributeValue   value)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; Parameter definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbParameter(void* const              userData,
                                  const OTF2_ParameterRef  self,
                                  const OTF2_StringRef     name,
                                  const OTF2_ParameterType parameterType)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Handle region flags
OTF2_CallbackCode
pearl::detail::otf2DefCbRegion(void* const           userData,
                               const OTF2_RegionRef  self,
                               const OTF2_StringRef  name,
                               const OTF2_StringRef  canonicalName,
                               const OTF2_StringRef  description,
                               const OTF2_RegionRole regionRole,
                               const OTF2_Paradigm   paradigm,
                               const OTF2_RegionFlag regionFlags,
                               const OTF2_StringRef  sourceFile,
                               const uint32_t        beginLineNumber,
                               const uint32_t        endLineNumber)
{
    CALLBACK_SETUP

    string        rname     = defs.getString(name).getString();
    const String& rfile     = defs.getString(sourceFile);
    Paradigm      rparadigm = otf2ConvertRegionParadigm(paradigm,
                                                        regionRole,
                                                        rname);
    Region::Role  rrole     = otf2ConvertRegionRole(paradigm,
                                                    regionRole,
                                                    rname);

    // Handle special regions
    DefsFactory::InternalRegionType type = DefsFactory::USER_REGION;
    if (rrole == Region::ROLE_ARTIFICIAL) {
        switch (native_value(rparadigm)) {
            case Paradigm::USER:
                if (rname == "MEASUREMENT OFF") {
                    type = DefsFactory::PAUSING_REGION;
                }
                break;

            case Paradigm::MEASUREMENT_SYSTEM:
                if (rname == "TRACE BUFFER FLUSH") {
                    type = DefsFactory::FLUSHING_REGION;
                }
                else if (rname == "THREADS") {
                    type = DefsFactory::THREADROOT_REGION;
                }
                else if ((rname == "TASKS") || (rname == "task_root")) {
                    type = DefsFactory::TASKROOT_REGION;
                }
                break;

            default:
                break;
        }
    }

    DefsFactory::instance()->createRegion(defs,
                                          self,
                                          canonicalName,
                                          name,
                                          description,
                                          rrole,
                                          rparadigm,
                                          sourceFile,
                                          beginLineNumber,
                                          endLineNumber,
                                          type);

    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; RmaWin definitions are currently ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbRmaWin(void* const          userData,
                               const OTF2_RmaWinRef self,
                               const OTF2_StringRef name,
                               const OTF2_CommRef   comm)
{
    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2DefCbSourceCodeLocation(void* const                      userData,
                                           const OTF2_SourceCodeLocationRef self,
                                           const OTF2_StringRef             file,
                                           const uint32_t                   lineNumber)
{
    CALLBACK_SETUP
    DefsFactory:: instance()->createSourceLocation(defs,
                                                   self,
                                                   file,
                                                   lineNumber);
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2DefCbString(void* const          userData,
                               const OTF2_StringRef self,
                               const char* const    str)
{
    CALLBACK_SETUP
    DefsFactory:: instance()->createString(defs,
                                           self,
                                           str);
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
pearl::detail::otf2DefCbSystemTreeNode(void* const                  userData,
                                       const OTF2_SystemTreeNodeRef self,
                                       const OTF2_StringRef         name,
                                       const OTF2_StringRef         className,
                                       const OTF2_SystemTreeNodeRef parent)
{
    CALLBACK_SETUP
    DefsFactory:: instance()->createSystemNode(defs,
                                               self,
                                               name,
                                               className,
                                               parent);
    CALLBACK_CLEANUP

    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; SystemTreeNodeDomain definitions are currently
///     ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbSystemTreeNodeDomain(void* const                  userData,
                                             const OTF2_SystemTreeNodeRef systemTreeNode,
                                             const OTF2_SystemTreeDomain  systemTreeDomain)
{
    return OTF2_CALLBACK_SUCCESS;
}


/// @todo
///     Implement callback; SystemTreeNodeProperty definitions are currently
///     ignored
OTF2_CallbackCode
pearl::detail::otf2DefCbSystemTreeNodeProperty(void* const                  userData,
                                               const OTF2_SystemTreeNodeRef systemTreeNode,
                                               const OTF2_StringRef         name,
                                               const OTF2_Type              type,
                                               const OTF2_AttributeValue    value)
{
    return OTF2_CALLBACK_SUCCESS;
}
