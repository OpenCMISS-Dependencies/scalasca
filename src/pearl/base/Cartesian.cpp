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
#include <pearl/Cartesian.h>

#include <pearl/Error.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class Cartesian
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

/**
 *  Creates a new instance and sets data to the given values.
 *
 *  @param id            Topology identifier
 *  @param num_locations Contains the number of locations in each dimension
 *  @param is_periodic   Contains boolean values defining whether the
 *                       corresponding dimension is periodic or not
 **/
Cartesian::Cartesian(uint32_t           id,
                     const cart_dims&   num_locations,
                     const cart_period& is_periodic)
  : Topology(id),
    m_num_locations(num_locations),
    m_is_periodic(is_periodic)
{
}


//--- Get & set cartesian topology information ------------------------------

/**
 *  Returns the number of dimensions of the cartesian grid.
 *
 *  @return Number of dimensions
 **/
uint32_t Cartesian::num_dimensions() const
{
  return m_num_locations.size();
}


/**
 *  Stores the number of locations in each dimension in the given vector
 *  @a outv.    
 *
 *  @param[out] outv Number of locations per dimension
 **/
void Cartesian::get_dimensionv(cart_dims& outv) const
{
  outv = m_num_locations;
}


/**
 *  Stores the periodicity of each dimension in the given vector @a outv.
 *
 *  @param[out] outv Periodicity per dimension
 **/
void Cartesian::get_periodv(cart_period& outv) const
{
  outv = m_is_periodic;
}


/**
 *  Stores the coordinate of the given @a location in the vector @a coords.
 *
 *  @param      location %Location in the system hierarchy
 *  @param[out] coords   Coordinate of the given location
 *
 *  @exception RuntimeError if the given @a location is not part of this
 *                          topology
 **/
void Cartesian::get_coords(const Location& location, cart_coords& coords) const
{
  CoordinateMap::const_iterator it = m_coords.find(location.getId());
  if (it == m_coords.end())
    throw RuntimeError("Cartesian::get_coords(const Location&,cart_coords&) -- "
                       "Coordinate not found.");

  coords = it->second;
}


//--- Private methods -------------------------------------------------------

/**
 *  Assigns the coordinate @a coords to the given @a location.
 *
 *  @param location %Location in the system hierarchy
 *  @param coords   Coordinate of the given location
 *
 *  @exception RuntimeError if one of the following conditions is met:
 *                            - The number of dimensions of the coordinate
 *                              and the cartesian topology do not match.
 *                            - The coordinate is out of bounds.
 *                            - The location coordinate is already defined.
 **/
void Cartesian::set_coords(const Location& location, const cart_coords& coords)
{
  size_t count = coords.size();

  if (count != m_num_locations.size())
    throw RuntimeError("Cartesian::set_coords(const Location&,const cart_coords&) -- "
                       "Dimensions do not match.");

  for (size_t i = 0; i < count; ++i)
    if (coords[i] >= m_num_locations[i])
      throw RuntimeError("Cartesian::set_coords(const Location&,const cart_coords&) -- "
                         "Coordinate out of bounds");

  CoordinateMap::iterator it = m_coords.find(location.getId());
  if (it != m_coords.end())
    throw RuntimeError("Cartesian::set_coords(const Location&,const cart_coords&) -- "
                       "Location coordinate already defined.");

  m_coords.insert(CoordinateMap::value_type(location.getId(), coords));
}
