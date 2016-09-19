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
#include <pearl/ThreadEnd_rep.h>

#include <iostream>

#include <pearl/Buffer.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class ThreadEnd_rep
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

ThreadEnd_rep::ThreadEnd_rep(timestamp_t timestamp,
                             uint32_t    threadContingent,
                             uint64_t    sequenceCount)
  : Event_rep(timestamp),
    mThreadContingent(threadContingent),
    mSequenceCount(sequenceCount)
{
}


ThreadEnd_rep::ThreadEnd_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  mThreadContingent = buffer.get_uint32();
  mSequenceCount    = buffer.get_uint64();    
}


//--- Event type information ------------------------------------------------

event_t ThreadEnd_rep::getType() const
{
  return THREAD_END;
}


bool ThreadEnd_rep::isOfType(event_t type) const
{
  return ((THREAD_END == type)
          || (GROUP_ALL == type));
}


//--- Access event data -----------------------------------------------------

uint64_t ThreadEnd_rep::getSequenceCount() const
{
  return mSequenceCount;
}


uint32_t ThreadEnd_rep::getThreadContingent() const
{
  return mThreadContingent;
}

//--- Serialize event data (protected) --------------------------------------

void ThreadEnd_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_uint32(mThreadContingent);
  buffer.put_uint64(mSequenceCount);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& ThreadEnd_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  sequence   = " << mSequenceCount << endl
                << "  contingent = " << mThreadContingent << endl;
}
