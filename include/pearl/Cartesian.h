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


#ifndef PEARL_CARTESIAN_H
#define PEARL_CARTESIAN_H


#include <map>
#include <vector>

#include "Location.h"
#include "Topology.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file    Cartesian.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class Cartesian.
 *
 *  This header file provides the declaration of the class Cartesian.
 **/
/*-------------------------------------------------------------------------*/

namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   Cartesian
 *  @ingroup PEARL_base
 *  @brief   Stores information related to cartesian grid topologies.
 *
 *  The class Cartesian provides information related to virtual cartesian
 *  grid topologies. These grids can have arbitrary dimensions. In addition,
 *  each dimension can be periodic or not (e.g., to define a torus topology).
 **/
/*-------------------------------------------------------------------------*/

class Cartesian : public Topology
{
  public:
    /// Container type for topology dimensions
    typedef std::vector<uint32_t> cart_dims;

    /// Container type for topology periodicity
    typedef std::vector<bool> cart_period;

    // Container type for cartesian coordinates
    typedef std::vector<uint32_t> cart_coords;


    /// @name Constructors & destructor
    /// @{

    Cartesian(uint32_t           id,
              const cart_dims&   num_locations,
              const cart_period& is_periodic);

    /// @}
    /// @name Get & set cartesian topology information
    /// @{

    uint32_t num_dimensions() const;

    void get_dimensionv(cart_dims& outv) const;
    void get_periodv(cart_period& outv) const;

    void get_coords(const Location& location, cart_coords& coords) const;
    void set_coords(const Location& location, const cart_coords& coords);

    /// @}


  private:
    /// Container type for the coordinate mapping
    typedef std::map<Location::IdType, cart_coords> CoordinateMap;


    /// Stores the number of locations in each dimension
    cart_dims m_num_locations;

    /// Stores the periodicity of each dimension
    cart_period m_is_periodic;

    /// Coordinate map (Location -@> grid coordinate)
    CoordinateMap m_coords;
};


}   // namespace pearl


#endif   // !PEARL_CARTESIAN_H
