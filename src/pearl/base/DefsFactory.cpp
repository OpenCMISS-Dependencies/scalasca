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


#include <config.h>
#include "DefsFactory.h"

#include <cassert>
#include <cstddef>

#include <elg_defs.h>

#include <pearl/CallingContext.h>
#include <pearl/Callpath.h>
#include <pearl/Callsite.h>
#include <pearl/Communicator.h>
#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/Location.h>
#include <pearl/Metric.h>
#include <pearl/RmaWindow.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>

#include "Process.h"
#include "UnknownLocationGroup.h"

using namespace std;
using namespace pearl;


//--- Constants -------------------------------------------------------------

// For Metric objects
const string str_integer("INTEGER");
const string str_counter("COUNTER");
const string str_start  ("START");


//--- Static member variables -----------------------------------------------

const DefsFactory* DefsFactory::m_instance = NULL;


//---------------------------------------------------------------------------
//
//  class DefsFactory
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

DefsFactory::~DefsFactory()
{
}


//--- Generic factory methods -----------------------------------------------

void DefsFactory::createCallingContext(GlobalDefs&       defs,
                                       const uint32_t    id,
                                       const uint32_t    regionId,
                                       const uint32_t    sclId,
                                       const uint32_t    parentId) const
{
  const CallingContext& parent = defs.getCallingContext(parentId);
  defs.addCallingContext(new CallingContext(id,
                                            defs.getRegion(regionId),
                                            defs.getSourceLocation(sclId),
                                            (parent != CallingContext::UNDEFINED)
                                              ? const_cast<CallingContext*>(&parent)
                                              : 0));
}


void DefsFactory::createCallpath(GlobalDefs& defs,
                                 const uint32_t id,
                                 const uint32_t regionId,
                                 const uint32_t callsiteId,
                                 const uint32_t parentId) const
{
  defs.addCallpath(new Callpath(id,
                                defs.getRegion(regionId),
                                defs.getCallsite(callsiteId),
                                defs.get_cnode(parentId)));
}


void DefsFactory::createCallsite(GlobalDefs&    defs,
                                 const uint32_t id,
                                 const uint32_t filenameId,
                                 const uint32_t line,
                                 const uint32_t regionId) const
{
  defs.addCallsite(new Callsite(id,
                                defs.getString(filenameId),
                                line,
                                defs.getRegion(regionId)));
}


void DefsFactory::createLocation(GlobalDefs&          defs,
                                 const uint64_t       id,
                                 const uint32_t       nameId,
                                 const Location::Type type,
                                 const uint64_t       numEvents,
                                 const uint32_t       parentId) const
{
  defs.addLocation(new Location(id,
                                defs.getString(nameId),
                                type,
                                numEvents,
                                defs.get_lgroup(parentId)));
}


void DefsFactory::createLocationGroup(GlobalDefs&               defs,
                                      const uint32_t            id,
                                      const uint32_t            nameId,
                                      const LocationGroup::Type type,
                                      const uint32_t            parentId) const
{
  switch (type) {
    case LocationGroup::TYPE_UNKNOWN:
      defs.addLocationGroup(new UnknownLocationGroup(id,
                                                     defs.getString(nameId),
                                                     defs.get_systemnode(parentId)));
      break;

    case LocationGroup::TYPE_PROCESS:
      defs.addLocationGroup(new Process(id,
                                        defs.getString(nameId),
                                        defs.get_systemnode(parentId)));
      break;

    default:
      assert(false);
      break;
  }
}


void DefsFactory::createRegion(GlobalDefs&              defs,
                               const uint32_t           id,
                               const uint32_t           canonicalNameId,
                               const uint32_t           nameId,
                               const uint32_t           descriptionId,
                               const Region::Role       role,
                               const Paradigm           paradigm,
                               const uint32_t           filenameId,
                               const uint32_t           startLine,
                               const uint32_t           endLine,
                               const InternalRegionType type) const
{
  defs.addRegion(new Region(id,
                            defs.getString(canonicalNameId),
                            defs.getString(nameId),
                            defs.getString(descriptionId),
                            role,
                            paradigm,
                            defs.getString(filenameId),
                            startLine,
                            endLine));

  if (DefsFactory::USER_REGION != type) {
    const Region& region = defs.getRegion(id);
    switch (type) {
      case DefsFactory::PAUSING_REGION:
        defs.setPausingRegion(const_cast<Region*>(&region));
        break;

      case DefsFactory::FLUSHING_REGION:
        defs.setFlushingRegion(const_cast<Region*>(&region));
        break;

      case DefsFactory::TASKROOT_REGION:
        defs.setTaskRootRegion(const_cast<Region*>(&region));
        break;

      case DefsFactory::THREADROOT_REGION:
        defs.setThreadRootRegion(const_cast<Region*>(&region));
        break;

      default:
        break;
    }
  }
}


void DefsFactory::createSourceLocation(GlobalDefs& defs,
                                       const uint32_t id,
                                       const uint32_t filenameId,
                                       const uint32_t line) const
{
  defs.addSourceLocation(new SourceLocation(id,
                                            defs.getString(filenameId),
                                            line));
}


void DefsFactory::createString(GlobalDefs&    defs,
                               const uint32_t id,
                               const string&  str) const
{
  defs.addString(new String(id, str));
}


void DefsFactory::createSystemNode(GlobalDefs&    defs,
                                   const uint32_t id,
                                   const uint32_t nameId,
                                   const uint32_t classId,
                                   const uint32_t parentId) const
{
  defs.addSystemNode(new SystemNode(id,
                                    defs.getString(nameId),
                                    defs.getString(classId),
                                    defs.get_systemnode(parentId)));
}


void DefsFactory::createMetric(GlobalDefs&   defs,
                               ident_t       id,
                               const string& name,
                               const string& descr,
                               uint8_t       type,
                               uint8_t       mode,
                               uint8_t       interval) const
{
  // Check parameter values
  if (type != ELG_INTEGER)
    throw FatalError("Unsupported metric data type.");
  if (mode != ELG_COUNTER)
    throw FatalError("Unsupported metric mode.");
  if (interval != ELG_START)
    throw FatalError("Unsupported metric interval semantics.");

  // Create & store new metric object
  defs.add_metric(new Metric(id,
                             name,
                             descr,
                             str_integer,
                             str_counter,
                             str_start));
}


void DefsFactory::createCartesian(GlobalDefs&        defs,
                                  ident_t            id,
                                  const cart_dims&   dimensions,
                                  const cart_period& periodic) const
{
  // Create & store new cartesian topology object
  defs.add_cartesian(new Cartesian(id, dimensions, periodic));
}


//--- MPI-specific factory methods ------------------------------------------

void DefsFactory::createMpiGroup(GlobalDefs&    defs,
                                 uint32_t       id,
                                 uint32_t       nameId,
                                 process_group& ranks,
                                 bool           isSelf,
                                 bool           isWorld) const
{
  // Create & store new group object
  defs.addProcessGroup(new ProcessGroup(id,
                                        defs.getString(nameId),
                                        ranks,
                                        isSelf,
                                        isWorld));
}


void DefsFactory::createMpiComm(GlobalDefs& defs,
                                uint32_t    id,
                                uint32_t    nameId,
                                uint32_t    groupId,
                                uint32_t    parentId) const
{
  // Create & store new communicator object
  defs.addCommunicator(new Communicator(id,
                                        defs.getString(nameId),
                                        defs.getProcessGroup(groupId),
                                        Paradigm::MPI,
                                        defs.get_comm(parentId)));
}


void DefsFactory::createMpiWindow(GlobalDefs& defs,
                                  ident_t     id,
                                  ident_t     comm_id) const
{
  // Create & store new RMA window object
  defs.add_window(new RmaWindow(id, defs.get_comm(comm_id)));
}


void DefsFactory::createMpiCartesian(GlobalDefs&        defs,
                                     ident_t            id,
                                     const cart_dims&   dimensions,
                                     const cart_period& periodic,
                                     ident_t            comm_id) const
{
  // Create "standard" cartesian topology
  createCartesian(defs, id, dimensions, periodic);
}


//--- Singleton interface ---------------------------------------------------

void DefsFactory::registerFactory(const DefsFactory* factory)
{
  assert(factory);

  if (m_instance)
    throw FatalError("DefsFactory::registerFactory(const DefsFactory*) -- "
                     "Factory already registered.");

  m_instance = factory;
}


void DefsFactory::deregisterFactory()
{
  if (!m_instance)
    throw FatalError("DefsFactory::deregisterFactory() -- "
                     "No factory registered.");

  delete m_instance;
  m_instance = 0;
}
