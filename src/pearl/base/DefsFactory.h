/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class DefsFactory.
 *
 *  This header file provides the declaration of the class DefsFactory.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_DEFSFACTORY_H
#define PEARL_DEFSFACTORY_H


#include <cassert>
#include <string>
#include <vector>

#include <pearl/Cartesian.h>
#include <pearl/Location.h>
#include <pearl/LocationGroup.h>
#include <pearl/ProcessGroup.h>
#include <pearl/Region.h>
#include <pearl/pearl_types.h>


namespace pearl
{
//--- Forward declarations --------------------------------------------------

class GlobalDefs;


/*-------------------------------------------------------------------------*/
/**
 *  @class   DefsFactory
 *  @ingroup PEARL_base
 *  @brief   Abstract factory class for global definition objects.
 **/
/*-------------------------------------------------------------------------*/

class DefsFactory
{
    public:
        /// Container type for process groups
        typedef ProcessGroup::RankContainer process_group;

        /// Container type for cartesian topology dimensions
        typedef Cartesian::cart_dims cart_dims;

        /// Container type for cartesian topology periodicity
        typedef Cartesian::cart_period cart_period;

        /// Internal region specifiers
        enum InternalRegionType
        {
            USER_REGION,       ///< User-defined region
            PAUSING_REGION,    ///< %Region where measurement was turned off
            FLUSHING_REGION,   ///< %Buffer flush region
            TASKROOT_REGION,   ///< %Task root region
            THREADROOT_REGION, ///< %Thread root region

            NUMBER_OF_TYPES ///< Not a valid type, for internal use only
        };


        /// @name Constructors & destructor
        /// @{

        virtual
        ~DefsFactory();

        /// @}
        /// @name Generic factory methods
        /// @{

        void
        createCallingContext(GlobalDefs& defs,
                             uint32_t    id,
                             uint32_t    regionId,
                             uint32_t    sclId,
                             uint32_t    parentId) const;
        void
        createCallpath(GlobalDefs& defs,
                       uint32_t    id,
                       uint32_t    regionId,
                       uint32_t    callsiteId,
                       uint32_t    parentId) const;
        void
        createCallsite(GlobalDefs& defs,
                       uint32_t    id,
                       uint32_t    filenameId,
                       uint32_t    line,
                       uint32_t    regionId) const;
        void
        createLocation(GlobalDefs&    defs,
                       uint64_t       id,
                       uint32_t       nameId,
                       Location::Type type,
                       uint64_t       numEvents,
                       uint32_t       parentId) const;
        void
        createLocationGroup(GlobalDefs&         defs,
                            uint32_t            id,
                            uint32_t            nameId,
                            LocationGroup::Type type,
                            uint32_t            parendId) const;
        void
        createRegion(GlobalDefs&        defs,
                     uint32_t           id,
                     uint32_t           canonicalNameId,
                     uint32_t           nameId,
                     uint32_t           descriptionId,
                     Region::Role       role,
                     Paradigm           paradigm,
                     uint32_t           filenameId,
                     uint32_t           startLine,
                     uint32_t           endLine,
                     InternalRegionType type) const;
        void
        createSourceLocation(GlobalDefs& defs,
                             uint32_t    id,
                             uint32_t    filenameId,
                             uint32_t    line) const;
        void
        createString(GlobalDefs&        defs,
                     uint32_t           id,
                     const std::string& str) const;
        void
        createSystemNode(GlobalDefs& defs,
                         uint32_t    id,
                         uint32_t    nameId,
                         uint32_t    classId,
                         uint32_t    parentId) const;

        void
        createMetric(GlobalDefs&        defs,
                     ident_t            id,
                     const std::string& name,
                     const std::string& descr,
                     uint8_t            type,
                     uint8_t            mode,
                     uint8_t            interval) const;
        void
        createCartesian(GlobalDefs&        defs,
                        ident_t            id,
                        const cart_dims&   dimensions,
                        const cart_period& periodic) const;

        /// @}
        /// @name MPI-specific factory methods
        /// @{

        virtual void
        createMpiGroup(GlobalDefs&    defs,
                       uint32_t       id,
                       uint32_t       nameId,
                       process_group& ranks,
                       bool           isSelf,
                       bool           isWorld) const;
        virtual void
        createMpiComm(GlobalDefs& defs,
                      uint32_t    id,
                      uint32_t    nameId,
                      uint32_t    groupId,
                      uint32_t    parentId) const;

        virtual void
        createMpiWindow(GlobalDefs& defs,
                        ident_t     id,
                        ident_t     comm_id) const;
        virtual void
        createMpiCartesian(GlobalDefs&        defs,
                           ident_t            id,
                           const cart_dims&   dimensions,
                           const cart_period& periodic,
                           ident_t            comm_id) const;

        /// @}
        /// @name Singleton interface
        /// @{

        static const DefsFactory*
        instance();
        static void
        registerFactory(const DefsFactory* factory);
        static void
        deregisterFactory();

        /// @}


    private:
        /// Single factory instance
        static const DefsFactory* m_instance;
};


//--- Inline methods --------------------------------------------------------

inline const DefsFactory*
DefsFactory::instance()
{
    assert(m_instance);
    return m_instance;
}
}   // namespace pearl


#endif   // !PEARL_DEFSFACTORY_H
