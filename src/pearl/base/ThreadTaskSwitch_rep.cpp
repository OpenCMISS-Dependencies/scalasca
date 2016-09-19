/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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
#include <pearl/ThreadTaskSwitch_rep.h>

#include <iostream>

#include <pearl/Buffer.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class ThreadTaskSwitch_rep
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

ThreadTaskSwitch_rep::ThreadTaskSwitch_rep(timestamp_t timestamp,
                                           uint32_t    threadTeam,
                                           uint64_t    taskId)
  : Event_rep(timestamp),
    mThreadTeam(threadTeam),
    mTaskId(taskId)
{
}


ThreadTaskSwitch_rep::ThreadTaskSwitch_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  mThreadTeam = buffer.get_uint32();
  mTaskId     = buffer.get_uint64();    
}


//--- Event type information ------------------------------------------------

event_t ThreadTaskSwitch_rep::getType() const
{
  return THREAD_TASK_SWITCH;
}


bool ThreadTaskSwitch_rep::isOfType(event_t type) const
{
  return ((THREAD_TASK_SWITCH == type)
          || (GROUP_ALL == type));
}


//--- Access event data -----------------------------------------------------

uint64_t ThreadTaskSwitch_rep::getTaskId() const
{
  return mTaskId;
}


uint32_t ThreadTaskSwitch_rep::getThreadTeam() const
{
  return mThreadTeam;
}


//--- Serialize event data (protected) --------------------------------------

void ThreadTaskSwitch_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_uint32(mThreadTeam);
  buffer.put_uint64(mTaskId);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& ThreadTaskSwitch_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  taskId = " << mTaskId << endl
                << "  teamId = " << mThreadTeam << endl;
}
