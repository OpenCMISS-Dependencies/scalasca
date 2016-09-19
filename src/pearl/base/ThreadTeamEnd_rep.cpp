/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2014                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include <pearl/ThreadTeamEnd_rep.h>

#include <iostream>

#include <pearl/Buffer.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class ThreadTeamEnd_rep
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

ThreadTeamEnd_rep::ThreadTeamEnd_rep(timestamp_t timestamp,
                                     uint32_t    threadTeam)
  : Event_rep(timestamp),
    mThreadTeam(threadTeam)
{
}


ThreadTeamEnd_rep::ThreadTeamEnd_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  mThreadTeam = buffer.get_uint32();
}


//--- Event type information ------------------------------------------------

event_t ThreadTeamEnd_rep::getType() const
{
  return THREAD_TEAM_END;
}


bool ThreadTeamEnd_rep::isOfType(event_t type) const
{
  return ((THREAD_TEAM_END == type)
          || (GROUP_ALL == type));
}

//--- Access event data -----------------------------------------------------

uint32_t ThreadTeamEnd_rep::getThreadTeam() const
{
  return mThreadTeam;
}


//--- Serialize event data (protected) --------------------------------------

void ThreadTeamEnd_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_uint32(mThreadTeam);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& ThreadTeamEnd_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  teamId = " << mThreadTeam << endl;
}
