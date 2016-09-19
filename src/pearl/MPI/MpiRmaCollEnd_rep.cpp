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
#include <pearl/MpiRmaCollEnd_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiWindow.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class MpiRmaCollEnd_rep
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

MpiRmaCollEnd_rep::MpiRmaCollEnd_rep(timestamp_t timestamp,
                                     MpiWindow*  window)
  : Event_rep(timestamp),
    m_window(window)
{
}


MpiRmaCollEnd_rep::MpiRmaCollEnd_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  m_window = dynamic_cast<MpiWindow*>(defs.get_window(buffer.get_id()));
}


//--- Event type information ------------------------------------------------

event_t MpiRmaCollEnd_rep::getType() const
{
  return MPI_RMA_COLLECTIVE_END;
}


bool MpiRmaCollEnd_rep::isOfType(event_t type) const
{
  return ((MPI_RMA_COLLECTIVE_END == type)
          || (GROUP_END == type)
          || (GROUP_ALL == type));
}


//--- Access event data -----------------------------------------------------

MpiWindow* MpiRmaCollEnd_rep::get_window() const
{
  return m_window;
}


//--- Serialize event data (protected) --------------------------------------

void MpiRmaCollEnd_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_id(m_window->get_id());
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiRmaCollEnd_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  win    = " << *m_window << endl;
}
