/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOUT_SYNCHPOINTHANDLER_H
#define SCOUT_SYNCHPOINTHANDLER_H

#include "AnalysisHandler.h"

#include <memory>

// --- Forward declarations -------------------------------------------------

namespace pearl
{
    class Event;
    class GlobalDefs;
    class ProcessGroup;
}

namespace scout
{

// --------------------------------------------------------------------------
/**
 * @class   SynchpointHandler
 * @ingroup scout
 * @brief   Synchronization-point detection callback routines
 * 
 * The SynchpointHandler handles synchronization-point detection 
 * for collective and point-to-point MPI communication
 */
// --------------------------------------------------------------------------

class SynchpointHandler : public AnalysisHandler
{
  class SpHImpl;

  std::auto_ptr<SpHImpl> mP;

public:

  SynchpointHandler();

  ~SynchpointHandler();

  bool              isSynchpoint         (const pearl::Event& e   ) const;
  bool              isWaitstate          (const pearl::Event& e   ) const;

  SynchpointInfo    getSynchpointInfo    (const pearl::Event& e   ) const;

  pearl::Event      findPreviousMpiSynchpoint(
                                          const pearl::Event&          e, 
                                          int                          rank) const;
  pearl::Event      findPreviousOmpSynchpoint(
                                          const pearl::Event&          e) const;
  pearl::Event      findPreviousMpiGroupSynchpoints(
                                          const pearl::Event&          from,
                                          const pearl::ProcessGroup&   group,
                                          int                          rank,
                                          uint32_t                     nprocs,
                                          std::map<int, pearl::Event>& spmap) const;

  SynchpointInfoMap getSynchpoints() const;
  SynchpointInfoMap getSynchpointsBetween(const pearl::Event& from, 
                                          const pearl::Event& to  ) const;

  void register_pre_pattern_callbacks (const CallbackManagerMap& cbm);
  void register_post_pattern_callbacks (const CallbackManagerMap& cbm);

};

} // namespace scout

#endif // !SCOUT_SYNCHPOINTHANDLER_H
