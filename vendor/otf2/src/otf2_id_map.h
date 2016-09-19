/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_INTERNAL_ID_MAP_H
#define OTF2_INTERNAL_ID_MAP_H


/**
 *  @file
 *
 *  @brief      This file provides all routines that write definition records
 *              of a single location.
 */


/* Actual ID mapping data structure */
struct OTF2_IdMap_struct
{
    OTF2_IdMapMode mode;         /* Mapping mode */
    uint64_t*      items;        /* Field elements */
    uint64_t       capacity;     /* Total field size */
    uint64_t       size;         /* Number of entries used */
};


size_t
otf2_id_map_get_size( const OTF2_IdMap* idMap );


void
otf2_id_map_write( OTF2_Buffer*      bufferHandle,
                   const OTF2_IdMap* idMap );


OTF2_ErrorCode
otf2_id_map_read( OTF2_Buffer*       bufferHandle,
                  const OTF2_IdMap** idMap );


#endif /* OTF2_INTERNAL_ID_MAP_H */
