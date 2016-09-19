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
#include <pearl/MpiRmaPutStart_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/GlobalDefs.h>
#include <pearl/MpiWindow.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class MpiRmaPutStart_rep
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

MpiRmaPutStart_rep::MpiRmaPutStart_rep(timestamp_t timestamp,
                                       uint32_t    rma_id,
                                       uint32_t    remote,
                                       uint64_t    bytesSent,
                                       MpiWindow*  window)
  : RmaPutStart_rep(timestamp, rma_id, remote, bytesSent),
    m_window(window)
{
}


MpiRmaPutStart_rep::MpiRmaPutStart_rep(const GlobalDefs& defs, Buffer& buffer)
  : RmaPutStart_rep(defs, buffer)
{
    m_window = dynamic_cast<MpiWindow*>(defs.get_window(buffer.get_id()));
}


//--- Event type information ------------------------------------------------

event_t MpiRmaPutStart_rep::getType() const
{
  return MPI_RMA_PUT_START;
}


bool MpiRmaPutStart_rep::isOfType(event_t type) const
{
  return ((MPI_RMA_PUT_START == type)
          || (RMA_PUT_START == type)
          || (GROUP_ALL == type));
}


//--- Access event data -----------------------------------------------------

MpiWindow* MpiRmaPutStart_rep::get_window() const
{
  return m_window;
}


//--- Serialize event data (protected) --------------------------------------

void MpiRmaPutStart_rep::pack(Buffer& buffer) const
{
  RmaPutStart_rep::pack(buffer);

  buffer.put_id(m_window->get_id());
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiRmaPutStart_rep::output(ostream& stream) const
{
  RmaPutStart_rep::output(stream);

  return stream << "  win      = " << *m_window << endl;
}
