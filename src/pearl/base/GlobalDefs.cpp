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


#include <config.h>
#include <pearl/GlobalDefs.h>

#include <cfloat>
#include <cstddef>
#include <cstdlib>
#include <algorithm>

#include <pearl/Cartesian.h>
#include <pearl/Error.h>
#include <pearl/Metric.h>
#include <pearl/RmaWindow.h>

#include "Calltree.h"
#include "ContextTree.h"
#include "Functors.h"
#include "SystemTree.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


//--- Utility functions -----------------------------------------------------

namespace
{

template<class containerT>
inline typename containerT::value_type get_entry(const containerT& container,
                                                 ident_t           id,
                                                 const string&     desc)
{
  if (id >= container.size()){
    throw RuntimeError("GlobalDefs::" + desc + " -- ID out of bounds.");
  }

  return container[id];
}


template<class containerT>
inline void add_entry(containerT&                     container,
                      typename containerT::value_type entry,
                      const string&                   desc)
{
  if (entry->get_id() == container.size())
    container.push_back(entry);
  else   // This should never happen!
    throw RuntimeError("GlobalDefs::" + desc + " -- Invalid ID.");
}


template<class containerT>
inline void addEntry(containerT&                     container,
                     typename containerT::value_type entry,
                     const string&                   desc)
{
  if (entry->getId() == container.size())
    container.push_back(entry);
  else   // This should never happen!
    throw RuntimeError("GlobalDefs::" + desc + " -- Invalid ID.");
}


inline bool locCmp(Location* item, Location::IdType id)
{
    return item->getId() < id;
}


inline bool commCmp(Communicator* item, Communicator::IdType id)
{
    return item->getId() < id;
}


inline bool procGrpCmp(ProcessGroup* item, ProcessGroup::IdType id)
{
    return item->getId() < id;
}


}   // unnamed namespace


//---------------------------------------------------------------------------
//
//  class GlobalDefs
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

GlobalDefs::GlobalDefs()
  : mGlobalOffset(DBL_MAX),
    mPausingRegion(NULL),
    mFlushingRegion(NULL),
    mTaskRootRegion(NULL),
    mThreadRootRegion(NULL)
{
  // Create call tree & system tree objects
  mCalltree    = new Calltree();
  mContextTree = new ContextTree();
  mSystemTree  = new SystemTree();
}


GlobalDefs::~GlobalDefs()
{
  // Delete entity containers
  // NOTE: The "MEASUREMENT OFF" and "TRACE BUFFER FLUSH" regions will be
  //       implicitly deleted.
  for_each(mCallsites.begin(), mCallsites.end(), delete_ptr<Callsite>());
  for_each(mCommunicators.begin(), mCommunicators.end(),
           delete_ptr<Communicator>());
  for_each(mLocations.begin(), mLocations.end(), delete_ptr<Location>());
  for_each(mLocationGroups.begin(), mLocationGroups.end(),
           delete_ptr<LocationGroup>());
  for_each(mProcessGroups.begin(), mProcessGroups.end(),
           delete_ptr<ProcessGroup>());
  for_each(mRegions.begin(), mRegions.end(), delete_ptr<Region>());
  for_each(mStrings.begin(), mStrings.end(), delete_ptr<String>());

  for_each(m_metrics.begin(), m_metrics.end(), delete_ptr<Metric>());
  for_each(m_cartesians.begin(), m_cartesians.end(), delete_ptr<Cartesian>());
  for_each(m_windows.begin(), m_windows.end(), delete_ptr<RmaWindow>());

  // Delete call tree, calling context tree, and system tree objects
  delete mCalltree;
  delete mContextTree;
  delete mSystemTree;
}


//--- Get number of stored definitions --------------------------------------

uint32_t GlobalDefs::numCallingContexts() const
{
  return mContextTree->numContexts();
}


uint32_t GlobalDefs::numCallpaths() const
{
  return mCalltree->numCallpaths();
}


uint32_t GlobalDefs::numCallsites() const
{
  return mCallsites.size();
}


uint32_t GlobalDefs::numCommunicators() const
{
  return mCommunicators.size();
}


uint32_t GlobalDefs::numLocations() const
{
  return mLocations.size();
}


uint32_t GlobalDefs::numLocationGroups() const
{
  return mLocationGroups.size();
}


uint32_t GlobalDefs::numProcessGroups() const
{
  return mProcessGroups.size();
}


uint32_t GlobalDefs::numRegions() const
{
  return mRegions.size();
}


uint32_t GlobalDefs::numSourceLocations() const
{
  return mSourceLocations.size();
}


uint32_t GlobalDefs::numStrings() const
{
  return mStrings.size();
}


uint32_t GlobalDefs::numSystemNodes() const
{
  return mSystemTree->numSystemNodes();
}


//--- Get stored definitions by ID ------------------------------------------

const CallingContext& GlobalDefs::getCallingContext(CallingContext::IdType id) const
{
  if (CallingContext::NO_ID == id)
    return CallingContext::UNDEFINED;

  CallingContext* entry = mContextTree->getContext(id);
  return *entry;
}


const Callpath& GlobalDefs::getCallpath(Callpath::IdType id) const
{
  if (Callpath::NO_ID == id)
    return Callpath::UNDEFINED;

  Callpath* entry = mCalltree->getCallpath(id);
  return *entry;
}


const Callsite& GlobalDefs::getCallsite(Callsite::IdType id) const
{
  if (Callsite::NO_ID == id)
    return Callsite::UNDEFINED;

  Callsite* entry = get_entry(mCallsites, id, "getCallsite(Callsite::IdType)");
  return *entry;
}


const Communicator& GlobalDefs::getCommunicator(Communicator::IdType id) const
{
    communicator_container::const_iterator it = lower_bound(mCommunicators.begin(),
                                                            mCommunicators.end(),
                                                            id,
                                                            ptr_fun(commCmp));
    if (mCommunicators.end() == it || (*it)->getId() != id)
        throw RuntimeError("GlobalDefs::getCommunicator(Communicator::IdType) -- Invalid ID.");

    return *(*it);
}


const Location& GlobalDefs::getLocation(Location::IdType id) const
{
    location_container::const_iterator it = lower_bound(mLocations.begin(),
                                                        mLocations.end(),
                                                        id,
                                                        ptr_fun(locCmp));
    if (mLocations.end() == it || (*it)->getId() != id)
        throw RuntimeError("GlobalDefs::getLocation(Location::IdType) -- Invalid ID.");

    return *(*it);
}


const LocationGroup& GlobalDefs::getLocationGroup(LocationGroup::IdType id) const
{
    LocationGroup* entry = get_entry(mLocationGroups, id,
                                     "getLocationGroup(LocationGroup::IdType)");
    return *entry;
}


const ProcessGroup& GlobalDefs::getProcessGroup(ProcessGroup::IdType id) const
{
    pgroup_container::const_iterator it = lower_bound(mProcessGroups.begin(),
                                                      mProcessGroups.end(),
                                                      id,
                                                      ptr_fun(procGrpCmp));
    if (mProcessGroups.end() == it || (*it)->getId() != id)
        throw RuntimeError("GlobalDefs::getProcessGroup(ProcessGroup::IdType) -- Invalid ID.");

    return *(*it);
}


const Region& GlobalDefs::getRegion(Region::IdType id) const
{
  if (Region::NO_ID == id)
    return Region::UNDEFINED;

  Region* entry = get_entry(mRegions, id, "getRegion(Region::IdType)");
  return *entry;
}


const SourceLocation& GlobalDefs::getSourceLocation(SourceLocation::IdType id) const
{
  if (SourceLocation::NO_ID == id)
    return SourceLocation::UNDEFINED;

  SourceLocation* entry = get_entry(mSourceLocations, id, "getSourceLocation(SourceLocation::IdType)");
  return *entry;
}


const String& GlobalDefs::getString(String::IdType id) const
{
  switch (id) {
    case String::NODE_ID:
      return String::NODE;

    case String::MACHINE_ID:
      return String::MACHINE;

    case String::NO_ID:
      return String::UNDEFINED;

    default:
      break;
  }

  String* entry = get_entry(mStrings, id, "getString(String::IdType)");
  return *entry;
}


const SystemNode& GlobalDefs::getSystemNode(SystemNode::IdType id) const
{
  if (SystemNode::NO_ID == id)
    return SystemNode::UNDEFINED;

  SystemNode* entry = mSystemTree->getSystemNode(id);
  return *entry;
}


const Location& GlobalDefs::getLocationByIndex(uint32_t index) const
{
  Location* entry = get_entry(mLocations, index, "getLocationByIndex(uint32_t)");
  return *entry;
}


const Communicator& GlobalDefs::getCommunicatorByIndex(uint32_t index) const
{
  Communicator* entry = get_entry(mCommunicators, index, "getCommunicatorByIndex(uint32_t)");
  return *entry;
}


Callpath* GlobalDefs::get_cnode(ident_t id) const
{
  if (id == PEARL_NO_ID)
    return NULL;

  Callpath* result = mCalltree->getCallpath(id);
  if (!result)
    throw RuntimeError("GlobalDefs::get_cnode(ident_t) -- ID out of bounds.");

  return result;
}


Metric* GlobalDefs::get_metric(ident_t id) const
{
  return get_entry(m_metrics, id, "get_metric(ident_t)");
}


ProcessGroup* GlobalDefs::get_group(ident_t id) const
{
  return get_entry(mProcessGroups, id, "get_group(ident_t)");
}


Communicator* GlobalDefs::get_comm(ident_t id) const
{
  if (Communicator::NO_ID == id)
    return NULL;

  communicator_container::const_iterator it = lower_bound(mCommunicators.begin(),
                                                          mCommunicators.end(),
                                                          id,
                                                          ptr_fun(commCmp));
  if (mCommunicators.end() == it || (*it)->getId() != id)
      throw RuntimeError("GlobalDefs::get_comm(ident_t) -- Invalid ID.");

  return (*it);
}


Cartesian* GlobalDefs::get_cartesian(ident_t id) const
{
  return get_entry(m_cartesians, id, "get_cartesian(ident_t)");
}


RmaWindow* GlobalDefs::get_window(ident_t id) const
{
  return get_entry(m_windows, id, "get_window(ident_t)");
}


SystemNode* GlobalDefs::get_systemnode(ident_t id) const
{
  return mSystemTree->getSystemNode(id);
}


LocationGroup* GlobalDefs::get_lgroup(ident_t id) const
{
  return get_entry(mLocationGroups, id, "get_lgroup(ident_t)");
}


//--- Get internal regions --------------------------------------------------

const Region& GlobalDefs::getPausingRegion() const
{
  return *mPausingRegion;
}


const Region& GlobalDefs::getFlushingRegion() const
{
  return *mFlushingRegion;
}


const Region& GlobalDefs::getTaskRootRegion() const
{
  return *mTaskRootRegion;
}


const Region& GlobalDefs::getThreadRootRegion() const
{
  return *mThreadRootRegion;
}


//--- Call tree handling ----------------------------------------------------

Calltree* GlobalDefs::get_calltree() const
{
  return mCalltree;
}


void GlobalDefs::set_calltree(Calltree* calltree)
{
  if (mCalltree == calltree)
    return;

  delete mCalltree;
  mCalltree = calltree;
}


//--- System tree handling --------------------------------------------------

SystemTree* GlobalDefs::get_systree() const
{
  return mSystemTree;
}


//--- Global time offset handling -------------------------------------------

timestamp_t GlobalDefs::getGlobalOffset() const
{
  return mGlobalOffset;
}


void GlobalDefs::setGlobalOffset(timestamp_t offset)
{
  mGlobalOffset = offset;
}


//--- Internal methods ------------------------------------------------------

/// @todo Check whether UNKNOWN callpath ist still necessary
void GlobalDefs::setup()
{
  // Create special "MEASUREMENT OFF" region if necessary
  if (!mPausingRegion) {
    // Create "MEASUREMENT OFF" string
    String::IdType stringId = numStrings();
    String*        pauseStr = new String(stringId, "MEASUREMENT OFF");
    addString(pauseStr);

    // Create "MEASUREMENT OFF" region
    Region::IdType regionId = numRegions();
    mPausingRegion = new Region(regionId,
                                *pauseStr,
                                *pauseStr,
                                String::UNDEFINED,
                                Region::ROLE_ARTIFICIAL,
                                Paradigm::USER,
                                String::UNDEFINED,
                                PEARL_NO_NUM,
                                PEARL_NO_NUM);
    addRegion(mPausingRegion);
  }

  // Create special "TRACE BUFFER FLUSH" region if necessary
  if (!mFlushingRegion) {
    // Create "TRACE BUFFER FLUSH" string
    String::IdType stringId = numStrings();
    String*        flushStr = new String(stringId, "TRACE BUFFER FLUSH");
    addString(flushStr);

    // Create "TRACE BUFFER FLUSH" region
    Region::IdType regionId = numRegions();
    mFlushingRegion = new Region(regionId,
                                 *flushStr,
                                 *flushStr,
                                 String::UNDEFINED,
                                 Region::ROLE_ARTIFICIAL,
                                 Paradigm::MEASUREMENT_SYSTEM,
                                 String::UNDEFINED,
                                 PEARL_NO_NUM,
                                 PEARL_NO_NUM);
    addRegion(mFlushingRegion);
  }

  // Create special "TASKS" region if necessary
  if (!mTaskRootRegion) {
    // Create "TASKS" string
    String::IdType stringId = numStrings();
    String*        taskRootStr = new String(stringId, "TASKS");
    addString(taskRootStr);

    // Create "TASKS" region
    Region::IdType regionId = numRegions();
    mTaskRootRegion = new Region(regionId,
                                 *taskRootStr,
                                 *taskRootStr,
                                 String::UNDEFINED,
                                 Region::ROLE_ARTIFICIAL,
                                 Paradigm::MEASUREMENT_SYSTEM,
                                 String::UNDEFINED,
                                 PEARL_NO_NUM,
                                 PEARL_NO_NUM);
    addRegion(mTaskRootRegion);
  }

  // Create special "THREADS" region if necessary
  if (!mThreadRootRegion) {
    // Create "THREADS" string
    String::IdType stringId = numStrings();
    String*        threadRootStr = new String(stringId, "THREADS");
    addString(threadRootStr);

    // Create "THREADS" region
    Region::IdType regionId = numRegions();
    mThreadRootRegion = new Region(regionId,
                                 *threadRootStr,
                                 *threadRootStr,
                                 String::UNDEFINED,
                                 Region::ROLE_ARTIFICIAL,
                                 Paradigm::MEASUREMENT_SYSTEM,
                                 *threadRootStr,
                                 PEARL_NO_NUM,
                                 PEARL_NO_NUM);
    addRegion(mThreadRootRegion);
  }

/*
  // Create "UNKNOWN" call path
  if (mCalltree->numCallpaths() == 0)
    mCalltree->getCallpath(Region::UNDEFINED, Callsite::UNDEFINED, NULL);
*/

  // Reset call tree modification flag
  mCalltree->setModified(false);
}


//--- Add new entities ------------------------------------------------------

void GlobalDefs::addCallingContext(CallingContext* context)
{
  mContextTree->addContext(context);
}


void GlobalDefs::addCallpath(Callpath* cnode)
{
  mCalltree->addCallpath(cnode);
}


void GlobalDefs::addCallsite(Callsite* callsite)
{
  addEntry(mCallsites, callsite, "addCallsite(Callsite*)");
}


void GlobalDefs::addCommunicator(Communicator* comm)
{
  // Simple case: first location or increasing location identifiers
  if (mCommunicators.empty()
      || mCommunicators.back()->getId() < comm->getId()) {
    mCommunicators.push_back(comm);
  }

  // Complex case: arbitrary location identifiers
  else {
    communicator_container::iterator it = lower_bound(mCommunicators.begin(),
                                                      mCommunicators.end(),
                                                      comm->getId(),
                                                      ptr_fun(commCmp));
    if (mCommunicators.end() == it || (*it)->getId() == comm->getId())
        throw RuntimeError("GlobalDefs::addCommunicator(Communicator*) -- Invalid ID.");

    mCommunicators.insert(it, comm);
  }
}


void GlobalDefs::addLocation(Location* location)
{
  // Simple case: first location or increasing location identifiers
  if (mLocations.empty()
      || mLocations.back()->getId() < location->getId()) {
    mLocations.push_back(location);
  }

  // Complex case: arbitrary location identifiers
  else {
    location_container::iterator it = lower_bound(mLocations.begin(),
                                                  mLocations.end(),
                                                  location->getId(),
                                                  ptr_fun(locCmp));
    if (mLocations.end() == it || (*it)->getId() == location->getId())
        throw RuntimeError("GlobalDefs::addLocation(Location*) -- Invalid ID.");

    mLocations.insert(it, location);
  }
}


void GlobalDefs::addLocationGroup(LocationGroup* group)
{
  addEntry(mLocationGroups, group, "addLocationGroup(LocationGroup*)");
}


void GlobalDefs::addProcessGroup(ProcessGroup* group)
{
  // Simple case: first location or increasing location identifiers
  if (mProcessGroups.empty()
      || mProcessGroups.back()->getId() < group->getId()) {
    mProcessGroups.push_back(group);
  }

  // Complex case: arbitrary location identifiers
  else {
    pgroup_container::iterator it = lower_bound(mProcessGroups.begin(),
                                                mProcessGroups.end(),
                                                group->getId(),
                                                ptr_fun(procGrpCmp));
    if (mProcessGroups.end() == it || (*it)->getId() == group->getId())
        throw RuntimeError("GlobalDefs::addProcessGroup(ProcessGroup*) -- Invalid ID.");

    mProcessGroups.insert(it, group);
  }
}


void GlobalDefs::addRegion(Region* region)
{
  addEntry(mRegions, region, "addRegion(Region*)");
}


void GlobalDefs::addSourceLocation(SourceLocation* scl)
{
  addEntry(mSourceLocations, scl, "addSourceLocation(SourceLocation*)");
}


void GlobalDefs::addString(String* str)
{
  addEntry(mStrings, str, "addString(String*)");
}


void GlobalDefs::addSystemNode(SystemNode* node)
{
  mSystemTree->addSystemNode(node);
}


void GlobalDefs::add_metric(Metric* metric)
{
  add_entry(m_metrics, metric, "add_metric(Metric*)");
}


void GlobalDefs::add_cartesian(Cartesian* cart)
{
  add_entry(m_cartesians, cart, "add_cartesian(Cartesian*)");
}


void GlobalDefs::add_window(RmaWindow* window)
{
  add_entry(m_windows, window, "add_window(RmaWindow*)");
}


//--- Set internal regions --------------------------------------------------

void GlobalDefs::setPausingRegion(Region* const region)
{
  if (mPausingRegion)
    throw FatalError("Duplicate definition of PAUSING region!");

  mPausingRegion = region;
}


void GlobalDefs::setFlushingRegion(Region* const region)
{
  if (mFlushingRegion)
    throw FatalError("Duplicate definition of FLUSHING region!");

  mFlushingRegion = region;
}

void GlobalDefs::setTaskRootRegion(Region* const region)
{
  if (mTaskRootRegion)
    throw FatalError("Duplicate definition of Task Root region!");

  mTaskRootRegion = region;
}


void GlobalDefs::setThreadRootRegion(Region* const region)
{
  if (mThreadRootRegion)
    throw FatalError("Duplicate definition of THREADS Root region!");

  mThreadRootRegion = region;
}
