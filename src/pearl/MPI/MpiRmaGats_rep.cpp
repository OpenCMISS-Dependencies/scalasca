/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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
#include <pearl/MpiRmaGats_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiGroup.h>
#include <pearl/MpiWindow.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class MpiRmaGats_rep
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

MpiRmaGats_rep::MpiRmaGats_rep(timestamp_t timestamp,
                               MpiWindow*  window,
                               MpiGroup*   group,
                               bool        sync)
  : Event_rep(timestamp),
    m_window(window),
    m_group(group),
    m_sync(sync)
{
}


MpiRmaGats_rep::MpiRmaGats_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  m_window = dynamic_cast<MpiWindow*>(defs.get_window(buffer.get_id()));
  m_group  = dynamic_cast<MpiGroup*>(defs.get_group(buffer.get_uint32()));
  m_sync   = (bool)buffer.get_uint8();
}


//--- Event type information ------------------------------------------------

event_t MpiRmaGats_rep::getType() const
{
  return MPI_RMA_GATS;
}


bool MpiRmaGats_rep::isOfType(event_t type) const
{
  return ((MPI_RMA_GATS == type)
          || (GROUP_ALL == type));
}


//--- Access event data -----------------------------------------------------

MpiWindow* MpiRmaGats_rep::get_window() const
{
  return m_window;
}


MpiGroup* MpiRmaGats_rep::get_group() const
{
  return m_group;
}


bool MpiRmaGats_rep::is_sync() const
{
    return m_sync;
}


//--- Serialize event data (protected) --------------------------------------

void MpiRmaGats_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_id(m_window->get_id());
  buffer.put_uint32(m_group->getId());
  buffer.put_uint8((uint8_t)(m_sync ? 1 : 0));
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiRmaGats_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  win    = " << *m_window << endl
                << "  group  = " << *m_group  << endl
                << "  sync   = " <<  m_sync   << endl;
}
