/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
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

/**
 *  @file
 *
 *  @brief      Internal functions for the OTF2_IdMap.
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <otf2/otf2.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "otf2_internal.h"
#include "otf2_file_types.h"
#include "otf2_reader.h"
#include "otf2_archive.h"

#include "otf2_attic_types.h"

#include "otf2_id_map.h"

#include "OTF2_File.h"
#include "OTF2_Buffer.h"

size_t
otf2_id_map_get_size( const OTF2_IdMap* idMap )
{
    UTILS_ASSERT( idMap );

    size_t size = 0;

    uint64_t number_of_entries = idMap->size;
    if ( idMap->mode == OTF2_ID_MAP_SPARSE )
    {
        number_of_entries /= 2;
    }
    size += otf2_buffer_size_uint64( number_of_entries );
    size += otf2_buffer_size_uint8( idMap->mode );

    for ( uint64_t i = 0; i < idMap->size; i++ )
    {
        size += otf2_buffer_size_uint64( idMap->items[ i ] );
    }

    return size;
}


void
otf2_id_map_write( OTF2_Buffer*      bufferHandle,
                   const OTF2_IdMap* idMap )
{
    UTILS_ASSERT( bufferHandle );
    UTILS_ASSERT( idMap );

    /* write the number of mappings not the number of entries in the items
       buffer */
    uint64_t number_of_entries = idMap->size;
    if ( idMap->mode == OTF2_ID_MAP_SPARSE )
    {
        number_of_entries /= 2;
    }
    OTF2_Buffer_WriteUint64( bufferHandle, number_of_entries );
    OTF2_Buffer_WriteUint8( bufferHandle, idMap->mode );

    for ( uint64_t i = 0; i < idMap->size; i++ )
    {
        OTF2_Buffer_WriteUint64( bufferHandle, idMap->items[ i ] );
    }
}

OTF2_ErrorCode
otf2_id_map_read( OTF2_Buffer*       bufferHandle,
                  const OTF2_IdMap** idMap )
{
    UTILS_ASSERT( bufferHandle );
    UTILS_ASSERT( idMap );

    OTF2_ErrorCode ret;

    uint64_t       map_size;
    OTF2_IdMapMode map_mode;

    ret = OTF2_Buffer_ReadUint64( bufferHandle, &map_size );
    if ( OTF2_SUCCESS != ret )
    {
        return UTILS_ERROR( ret, "Could not read size of IdMap. Invalid compression size." );
    }
    OTF2_Buffer_ReadUint8( bufferHandle, &map_mode );

    /* Read the complete map */
    OTF2_IdMap* id_map = OTF2_IdMap_Create( map_mode, map_size );
    if ( id_map == NULL )
    {
        return UTILS_ERROR( OTF2_ERROR_INTEGRITY_FAULT,
                            "ID map creation failed!" );
    }

    if ( map_mode == OTF2_ID_MAP_DENSE )
    {
        for ( uint64_t i = 0; i < map_size; i++ )
        {
            uint64_t global_id;
            ret = OTF2_Buffer_ReadUint64( bufferHandle, &global_id );
            if ( OTF2_SUCCESS != ret )
            {
                return UTILS_ERROR( ret, "Could not read global identifier of IdMap. Invalid compression size." );
            }
            OTF2_IdMap_AddIdPair( id_map, i, global_id );
        }
    }
    else
    {
        for ( uint64_t i = 0; i < map_size; i++ )
        {
            uint64_t local_id;
            uint64_t global_id;
            ret = OTF2_Buffer_ReadUint64( bufferHandle, &local_id );
            if ( OTF2_SUCCESS != ret )
            {
                return UTILS_ERROR( ret, "Could not read local identifier of IdMap. Invalid compression size." );
            }
            ret = OTF2_Buffer_ReadUint64( bufferHandle, &global_id );
            if ( OTF2_SUCCESS != ret )
            {
                return UTILS_ERROR( ret, "Could not read global identifier of IdMap. Invalid compression size." );
            }
            OTF2_IdMap_AddIdPair( id_map, local_id, global_id );
        }
    }

    *idMap = id_map;

    return OTF2_SUCCESS;
}
