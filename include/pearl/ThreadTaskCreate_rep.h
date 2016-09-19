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


#ifndef PEARL_THREADTASKCREATE_REP_H
#define PEARL_THREADTASKCREATE_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    ThreadTaskCreate_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class ThreadTaskCreate_rep.
 *
 *  This header file provides the declaration of the class
 *  ThreadTaskCreate_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 *  @class   ThreadTaskCreate_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for THREAD_TASK_CREATE events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR ThreadTaskCreate_rep : public Event_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    ThreadTaskCreate_rep(timestamp_t timestamp, 
                         uint32_t    threadTeam, 
                         uint64_t    taskId);
    ThreadTaskCreate_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t getType() const;
    virtual bool    isOfType(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    uint64_t getTaskId() const;

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
    /// Task identifier
    uint64_t mTaskId;
    
    /// Thread team identifier
    uint32_t mThreadTeam;
};


}   // namespace pearl


#endif   // !PEARL_THREADTASKCREATE_REP_H
