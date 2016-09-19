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


#ifndef PEARL_THREADTEAMBEGIN_REP_H
#define PEARL_THREADTEAMBEGIN_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    ThreadTeamBegin_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class ThreadTeamBegin_rep.
 *
 *  This header file provides the declaration of the class
 *  ThreadTeamBegin_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 *  @class   ThreadTeamBegin_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for THREAD_TEAM_BEGIN events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR ThreadTeamBegin_rep : public Event_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    ThreadTeamBegin_rep(timestamp_t timestamp, uint32_t threadTeam);
    ThreadTeamBegin_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t getType() const;
    virtual bool    isOfType(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

   uint32_t getThreadTeam() const;

    /// @}


  protected:
    /// @name Serialize event data
    /// @{

    virtual void pack(Buffer& buffer) const;

    /// @}
    /// @name Generate human-readable output of event data
    /// @{

    virtual std::ostream& output(std::ostream& stream) const;

    /// @}


  private:
    /// Thread team identifier
    uint32_t mThreadTeam;
};


}   // namespace pearl


#endif   // !PEARL_THREADTEAMBEGIN_REP_H
