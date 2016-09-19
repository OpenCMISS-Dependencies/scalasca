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
#include <pearl/ThreadTaskComplete_rep.h>

#include <iostream>

#include <pearl/Buffer.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class ThreadTaskComplete_rep
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

ThreadTaskComplete_rep::ThreadTaskComplete_rep(timestamp_t timestamp,
                                               uint32_t    threadTeam,
                                               uint64_t    taskId)
  : Event_rep(timestamp),
    mThreadTeam(threadTeam),
    mTaskId(taskId)
{
}


ThreadTaskComplete_rep::ThreadTaskComplete_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  mThreadTeam = buffer.get_uint32();
  mTaskId     = buffer.get_uint64();    
}


//--- Event type information ------------------------------------------------

event_t ThreadTaskComplete_rep::getType() const
{
  return THREAD_TASK_COMPLETE;
}


bool ThreadTaskComplete_rep::isOfType(event_t type) const
{
  return ((THREAD_TASK_COMPLETE == type)
          || (GROUP_ALL == type));
}


//--- Access event data -----------------------------------------------------

uint64_t ThreadTaskComplete_rep::getTaskId() const
{
  return mTaskId;
}


uint32_t ThreadTaskComplete_rep::getThreadTeam() const
{
  return mThreadTeam;
}


//--- Serialize event data (protected) --------------------------------------

void ThreadTaskComplete_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_uint32(mThreadTeam);
  buffer.put_uint64(mTaskId);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& ThreadTaskComplete_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  taskId = " << mTaskId << endl
                << "  teamId = " << mThreadTeam << endl;
}
