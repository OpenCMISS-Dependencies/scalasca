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
#include <pearl/ThreadBegin_rep.h>

#include <iostream>

#include <pearl/Buffer.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class ThreadBegin_rep
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

ThreadBegin_rep::ThreadBegin_rep(timestamp_t timestamp,
                                 uint32_t    threadContingent,
                                 uint64_t    sequenceCount)
  : Event_rep(timestamp),
    mThreadContingent(threadContingent),
    mSequenceCount(sequenceCount)
{
}


ThreadBegin_rep::ThreadBegin_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  mThreadContingent = buffer.get_uint32();
  mSequenceCount    = buffer.get_uint64();    
}


//--- Event type information ------------------------------------------------

event_t ThreadBegin_rep::getType() const
{
  return THREAD_BEGIN;
}


bool ThreadBegin_rep::isOfType(event_t type) const
{
  return ((THREAD_BEGIN == type)
          || (GROUP_ALL == type));
}


//--- Access event data -----------------------------------------------------

uint64_t ThreadBegin_rep::getSequenceCount() const
{
  return mSequenceCount;
}


uint32_t ThreadBegin_rep::getThreadContingent() const
{
  return mThreadContingent;
}


//--- Serialize event data (protected) --------------------------------------

void ThreadBegin_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_uint32(mThreadContingent);
  buffer.put_uint64(mSequenceCount);
}



//--- Generate human-readable output of event data (protected) --------------

ostream& ThreadBegin_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  sequence   = " << mSequenceCount << endl 
                << "  contingent = " << mThreadContingent << endl;
}
