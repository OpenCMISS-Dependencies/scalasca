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


#ifndef PEARL_GLOBALDEFS_H
#define PEARL_GLOBALDEFS_H


#include <vector>

#include "CallingContext.h"
#include "Callpath.h"
#include "Callsite.h"
#include "Communicator.h"
#include "Location.h"
#include "LocationGroup.h"
#include "ProcessGroup.h"
#include "Region.h"
#include "SourceLocation.h"
#include "String.h"
#include "SystemNode.h"
#include "pearlfwd_detail.h"
#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file    GlobalDefs.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class GlobalDefs.
 *
 *  This header file provides the declaration of the class GlobalDefs.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Calltree;
class Cartesian;
class LocalTrace;
class Metric;
class RmaWindow;
class SystemTree;


/*-------------------------------------------------------------------------*/
/**
 *  @class   GlobalDefs
 *  @ingroup PEARL_base
 *  @brief   Stores the global definitions of a tracing experiment.
 **/
/*-------------------------------------------------------------------------*/

class GlobalDefs
{
  public:
    /// @name Constructors & destructor
    /// @{

    ~GlobalDefs();

    /// @}
    /// @name Get number of stored definitions
    /// @{

    uint32_t numCallingContexts() const;
    uint32_t numCallpaths() const;
    uint32_t numCallsites() const;
    uint32_t numCommunicators() const;
    uint32_t numLocations() const;
    uint32_t numLocationGroups() const;
    uint32_t numProcessGroups() const;
    uint32_t numRegions() const;
    uint32_t numSourceLocations() const;
    uint32_t numStrings() const;
    uint32_t numSystemNodes() const;

    uint32_t num_metrics() const    { return m_metrics.size();    }
    uint32_t num_cartesians() const { return m_cartesians.size(); }
    uint32_t num_windows() const    { return m_windows.size();    }

    /// @}
    /// @name Get stored definitions by ID
    /// @{

    const CallingContext& getCallingContext(CallingContext::IdType id) const;
    const Callpath& getCallpath(Callpath::IdType id) const;
    const Callsite& getCallsite(Callsite::IdType id) const;
    const Communicator& getCommunicator(Communicator::IdType id) const;
    const Location& getLocation(Location::IdType id) const;
    const LocationGroup& getLocationGroup(LocationGroup::IdType id) const;
    const ProcessGroup& getProcessGroup(ProcessGroup::IdType id) const;
    const Region& getRegion(Region::IdType id) const;
    const SourceLocation& getSourceLocation(SourceLocation::IdType id) const;
    const String& getString(String::IdType id) const;
    const SystemNode& getSystemNode(SystemNode::IdType id) const;

    const Location&     getLocationByIndex(uint32_t index) const;
    const Communicator& getCommunicatorByIndex(uint32_t index) const;

    Callpath*  get_cnode(ident_t id) const;
    Metric*    get_metric(ident_t id) const;
    ProcessGroup* get_group(ident_t id) const;
    Communicator* get_comm(ident_t id) const;
    Cartesian* get_cartesian(ident_t id) const;
    RmaWindow* get_window(ident_t id) const;
    SystemNode* get_systemnode(ident_t id) const;
    LocationGroup* get_lgroup(ident_t id) const;

    /// @}
    /// @name Get internal regions
    /// @{

    const Region& getPausingRegion() const;
    const Region& getFlushingRegion() const;
    const Region& getTaskRootRegion() const;
    const Region& getThreadRootRegion() const;

    /// @}
    /// @name Call tree handling
    /// @{

    Calltree* get_calltree() const;
    void      set_calltree(Calltree* calltree);

    /// @}
    /// @name System tree handling
    /// @{

    SystemTree* get_systree() const;

    /// @}
    /// @name Global time offset handling
    /// @{

    timestamp_t getGlobalOffset() const;
    void        setGlobalOffset(timestamp_t offset);

    /// @}


  private:
    /// Container type for callsite definitions
    typedef std::vector<Callsite*> callsite_container;

    /// Container type for communicator definitions
    typedef std::vector<Communicator*> communicator_container;

    /// Container type for location definitions
    typedef std::vector<Location*> location_container;

    /// Container type for location group definitions
    typedef std::vector<LocationGroup*> lgroup_container;

    /// Container type for process group definitions
    typedef std::vector<ProcessGroup*> pgroup_container;

    /// Container type for region definitions
    typedef std::vector<Region*> region_container;

    /// Container type for source-code location definitions
    typedef std::vector<SourceLocation*> scl_container;

    /// Container type for string definitions
    typedef std::vector<String*> string_container;



    /// Container type for metric definitions
    typedef std::vector<Metric*> metric_container;

    /// Container type for cartesian topology definitions
    typedef std::vector<Cartesian*> cartesian_container;

    /// Container type for RMA window definitions
    typedef std::vector<RmaWindow*> window_container;


    /// %Callsite definitions: callsite ID |-@> callsite
    callsite_container mCallsites;

    /// Global call tree
    Calltree* mCalltree;

    /// Communicator definitions: communicator id |-@> communicator
    communicator_container mCommunicators;

    /// Global calling context tree
    detail::ContextTree* mContextTree;

    /// %Location definitions: location ID |-@> location
    location_container mLocations;

    /// %Location group definitions: group ID |-@> group
    lgroup_container mLocationGroups;

    /// %Process group definitions: group id |-@> group
    pgroup_container mProcessGroups;

    /// %Region definitions: region ID |-@> region
    region_container mRegions;

    /// Source-code location definitions: scl ID |-@> source-code location
    scl_container mSourceLocations;

    /// %String definitions: string ID |-@> string
    string_container mStrings;

    /// Global system hierarchy
    SystemTree* mSystemTree;

    /// Global time offset subtracted during trace preprocessing
    timestamp_t mGlobalOffset;


    /// %Metric definitions: metric id |-@> metric
    metric_container m_metrics;

    /// %Cartesian topology definitions: topology id |-@> cartesian
    cartesian_container m_cartesians;

    /// RMA window definitions: window id |-@> window
    window_container m_windows;

    /// Pointer to a Region object representing code regions where measurement
    /// was temporarily turned off by the user.
    Region* mPausingRegion;

    /// Pointer to a Region object representing a trace buffer flush triggered
    /// by the measurement system.
    Region* mFlushingRegion;

    /// Pointer to a Region object representing the task root in the 
    /// generic tasking model
    Region* mTaskRootRegion;

    /// Pointer to a Region object representing the THREADS root node for
    /// create/wait threading models
    Region* mThreadRootRegion;


    /// @name Constructors & destructor
    /// @{

    GlobalDefs();

    /// @}
    /// @name Internal methods
    /// @{

    void setup();

    /// @}
    /// @name Add new entities
    /// @{

    void addCallingContext(CallingContext* context);
    void addCallpath(Callpath* cnode);
    void addCallsite(Callsite* callsite);
    void addCommunicator(Communicator* comm);
    void addLocation(Location* location);
    void addLocationGroup(LocationGroup* group);
    void addProcessGroup(ProcessGroup* group);
    void addRegion(Region* region);
    void addSourceLocation(SourceLocation* scl);
    void addString(String* str);
    void addSystemNode(SystemNode* node);

    void add_metric(Metric* metric);
    void add_cartesian(Cartesian* cart);
    void add_window(RmaWindow*);

    /// @}
    /// @name Set internal regions
    /// @{

    void setPausingRegion(Region* region);
    void setFlushingRegion(Region* region);
    void setTaskRootRegion(Region* region);
    void setThreadRootRegion(Region* region);

    /// @}


    /* Declare friends */
    friend class LocalTrace;
    friend class DefsFactory;
    friend class MpiDefsFactory;
    friend class TraceArchive;
};


}   // namespace pearl


#endif   // !PEARL_GLOBALDEFS_H
