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


#ifndef CLC_NETWORK_H
#define CLC_NETWORK_H


#include <pearl/pearl_types.h>
#include "clc_types.h"


class Latency;


class Network
{
  public:
    // Constructors and Destructor
    Network(clc_location loc, Latency* lat);
    ~Network();

    pearl::timestamp_t get_latency(clc_location &loc, clc_event_t com_type);

  private:
    // Make instances uncopyable
    Network(const Network& rhs);
    Network& operator=(const Network& rhs);


    clc_location location;
    Latency*     latency;
};


#endif   // !CLC_NETWORK_H
