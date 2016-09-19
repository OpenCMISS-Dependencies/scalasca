/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/**
 * \file cubew_tar_writing.c
 * \brief Provides a file name transformation scheme: name -> tar file + shift. Produces tared cube report.
 *
 *
 *
 */

#ifndef __CUBEW_TAR_WRITING_C
#define __CUBEW_TAR_WRITING_C 0

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include <config.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cubew_report_layouts.h"
#include "cubew_tar_writing.h"
#include "cubew_meta_data_writer.h"
#include "cubew_compat_platform.h"



FILE*
cube_report_anchor_start( report_layout_writer* tar_writer )
{
    /* if metric is not finished yet, one has to */

    if ( tar_writer == NULL )
    {
        fprintf( stderr, "Non stanard run. Create faked tar_writer with temp name of cube \"__NOFILE__\". \n" );
        cube_writing_start( "__NOFILE__", CUBE_MASTER );
    }
    char* anchorname = cube_get_path_to_anchor( tar_writer->cubename );
    tar_writer->actual_tar_header = __cube_create_tar_header( tar_writer, anchorname );
    free( anchorname );
    tar_writer->header_position = ftell( tar_writer->tar );
    fwrite( tar_writer->actual_tar_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
    tar_writer->file_start_position = ftell( tar_writer->tar );
    tar_writer->anchor_writing      = CUBE_TRUE;
    return tar_writer->tar;
}




FILE*
cube_report_metric_data_start( report_layout_writer* tar_writer, cube_metric* met )
{
    if ( tar_writer == NULL )
    {
        fprintf( stderr, "Non stanard run. Create faked tar_writer with temp name of cube \"__NOFILE__\". \n" );
        cube_writing_start( "__NOFILE__", CUBE_MASTER );
    }
    char* dataname = cube_get_path_to_metric_data( tar_writer->cubename, met );
    tar_writer->actual_tar_header = __cube_create_tar_header( tar_writer, dataname );
    free( dataname );
    tar_writer->header_position = ftell( tar_writer->tar );
    fwrite( tar_writer->actual_tar_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
    tar_writer->file_start_position = ftell( tar_writer->tar );
    met->start_pos_of_datafile      = tar_writer->file_start_position;
    return tar_writer->tar;
}


FILE*
cube_report_metric_index_start( report_layout_writer* tar_writer, cube_metric* met )
{
    if ( tar_writer == NULL )
    {
        fprintf( stderr, "Non stanard run. Create faked tar_writer with temp name of cube \"__NOFILE__\". \n" );
        cube_writing_start( "__NOFILE__", CUBE_MASTER );
    }

    char* indexname = cube_get_path_to_metric_index( tar_writer->cubename, met );
    tar_writer->actual_tar_header = __cube_create_tar_header( tar_writer, indexname );
    free( indexname );
    tar_writer->header_position = ftell( tar_writer->tar );
    fwrite( tar_writer->actual_tar_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
    tar_writer->file_start_position = ftell( tar_writer->tar );
    return tar_writer->tar;
}


FILE*
cube_report_misc_data_start( report_layout_writer* tar_writer, const char* dataname )
{
    if ( tar_writer == NULL )
    {
        fprintf( stderr, "Non stanard run. Create faked tar_writer with temp name of cube \"__NOFILE__\". \n" );
        cube_writing_start( "__NOFILE__", CUBE_MASTER );
    }
    else
    {
        if ( tar_writer->actual_metric != NULL )
        {
            cube_metric_finish( tar_writer->actual_metric, 1 );  /* 1 - signalizes, that writing will be  done. This call can be performed only by CUBE_MASTER */
        }
        tar_writer->actual_metric = NULL;

        // from here we finished writing metric.

        char* _dataname = cube_get_path_to_misc_data( tar_writer->cubename, dataname );
        tar_writer->actual_tar_header = __cube_create_tar_header( tar_writer, _dataname );
        free( _dataname );
        tar_writer->header_position = ftell( tar_writer->tar );
        fwrite( tar_writer->actual_tar_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
        tar_writer->file_start_position = ftell( tar_writer->tar );
    }
    return tar_writer->tar;
}

void
cube_report_metric_data_finish( report_layout_writer* tar_writer, cube_metric* met, FILE* file )
{
    if ( file == NULL )
    {
//      fprintf(stderr, "WARNING: Pointer on file in the call \"cube_report_metric_data_finish\" is NULL. \n");
    }

    if ( cubew_fseeko( tar_writer->tar, 0, SEEK_END ) != 0 )
    {
        fprintf( stderr, "Cannot seek to the end of the data  file to finish its writing in the tared cube file %s. \n", tar_writer->actual_tar_file );
        perror( "The following error occurred" );
    }
    ;
    /* check for finish*/
    uint64_t size = ftell( tar_writer->tar ) - ( tar_writer->file_start_position );
    __cube_tar_file_finish( tar_writer, size );
}


void
cube_report_misc_data_finish( report_layout_writer* tar_writer, FILE* file )
{
    if ( file == NULL )
    {
//      fprintf(stderr, "WARNING: Pointer on file in the call \"cube_report_metric_data_finish\" is NULL. \n");
    }

    if ( cubew_fseeko( tar_writer->tar, 0, SEEK_END ) != 0 )
    {
        fprintf( stderr, "Cannot seek to the end of the data  file to finish its writing in the tared cube file %s. \n", tar_writer->actual_tar_file );
        perror( "The following error occurred" );
    }
    ;
    /* check for finish*/
    uint64_t size = ftell( tar_writer->tar ) - ( tar_writer->file_start_position );
    __cube_tar_file_finish( tar_writer, size );
}

void
cube_report_metric_data_weak_finish( report_layout_writer* tar_writer, cube_metric* met, FILE* file )
{
    if ( tar_writer == NULL )
    {
//      fprintf(stderr, "WARNING: Pointer on tar_writer in the call \"cube_report_metric_data_weak_finish\" is NULL. \n");
    }
    if ( met == NULL )
    {
//      fprintf(stderr, "WARNING: Pointer on metric in the call \"cube_report_metric_data_weak_finish\" is NULL. \n");
    }
    if ( file == NULL )
    {
//      fprintf(stderr, "WARNING: Pointer on file in the call \"cube_report_metric_data_weak_finish\" is NULL. \n");
    }
    return; /*  in tar case this does nothing*/
}


void
cube_report_metric_index_finish( report_layout_writer* tar_writer, cube_metric* met, FILE* file )
{
    if ( met == NULL )
    {
//      fprintf(stderr, "WARNING: Pointer on metric in the call \"cube_report_metric_data_weak_finish\" is NULL. \n");
    }
    if ( file == NULL )
    {
//      fprintf(stderr, "WARNING: Pointer on file in the call \"cube_report_metric_data_weak_finish\" is NULL. \n");
    }

    if ( cubew_fseeko( tar_writer->tar, 0, SEEK_END ) != 0 )
    {
        fprintf( stderr, "Cannot seek to the end of index file to finish its writing in the tared cube file %s. \n", tar_writer->actual_tar_file );
        perror( "The following error occurred" );
    }
    ;
    uint64_t size = ftell( tar_writer->tar ) - ( tar_writer->file_start_position );
    __cube_tar_file_finish( tar_writer, size );
}


void
cube_report_anchor_finish( report_layout_writer* tar_writer, FILE* file )
{
    if ( file == NULL )
    {
//      fprintf(stderr, "WARNING: Pointer on file in the call \"cube_report_metric_data_weak_finish\" is NULL. \n");
    }

    if ( cubew_fseeko( tar_writer->tar, 0, SEEK_END ) != 0 )
    {
        fprintf( stderr, "Cannot seek to the end of anchor file to finish its writing in the tared cube file %s. \n", tar_writer->actual_tar_file );
        perror( "The following error occurred" );
    }
    ;

    uint64_t size = ftell( tar_writer->tar ) - ( tar_writer->file_start_position );
    __cube_tar_file_finish( tar_writer, size );
}






report_layout_writer*
cube_writing_start( char* cubename, enum CubeFlavours_t cf )
{
    report_layout_writer* tar_writer = ( tar_writer_t* )calloc( 1, sizeof( tar_writer_t ) );
    tar_writer->cubename = cubew_strdup( cubename );
    tar_writer->mode     = cubew_strdup( "0000600" );
    tar_writer->username = cubew_strdup( getenv( "USER" ) );
    if ( tar_writer->username == NULL )
    {
        tar_writer->username = cubew_strdup( getenv( "LOGNAME" ) );
    }
    if ( tar_writer->username == NULL )
    {
        tar_writer->username = cubew_strdup( "nouser" );
    }

    tar_writer->group = ( char* )calloc( 32, sizeof( char ) );
    strcpy( tar_writer->group, "users" );
    tar_writer->uid             = getuid();
    tar_writer->gid             = getgid();
    tar_writer->actual_tar_file = __cube_get_tared_cube_name( cubename );
    tar_writer->tar             = fopen( tar_writer->actual_tar_file, "wb" );
    if ( tar_writer->tar == NULL )
    {
        fprintf( stderr, "Cannot open tared cube file %s. \n", tar_writer->actual_tar_file );
        perror( "The following error occurred" );
        fprintf( stderr, "Return NULL.\n" );
    }
    tar_writer->actual_metric       = NULL;
    tar_writer->actual_tar_header   = NULL;
    tar_writer->header_position     = 0;
    tar_writer->file_start_position = 0;
    tar_writer->anchor_writing      = CUBE_FALSE;
    tar_writer->cube_flavour        = cf;
    return tar_writer;
}




void
cube_report_write_metric_row( report_layout_writer* tar_writer, cube_metric* met )
{
    if ( tar_writer->cube_flavour == CUBE_SLAVE )
    {
        return;
    }
    if ( tar_writer->actual_metric == met )
    {
        return;
    }
    if ( tar_writer->actual_metric != NULL )
    {
        cube_metric_finish( tar_writer->actual_metric, 1 );  /* 1 - signalizes, that writing will be  done. This call can be performed only by CUBE_MASTER */
    }
    met->data_file            = cube_report_metric_data_start( tar_writer, met );
    tar_writer->actual_metric = met;
}




void
__cube_tar_finish( report_layout_writer* tar_writer )
{
    tar_empty_block* block = ( tar_empty_block* )calloc( 1, sizeof( tar_empty_block ) );
//     memset(block->block, 0, sizeof(tar_empty_block));
    fwrite( ( char* )block, 1, sizeof( tar_empty_block ), tar_writer->tar );
    fwrite( ( char* )block, 1, sizeof( tar_empty_block ), tar_writer->tar );
    free( block );
}



report_layout_writer*
cube_writing_end( report_layout_writer* tar_writer )
{
    if ( tar_writer->cube_flavour != CUBE_MASTER )
    {
        return NULL;
    }
    __cube_tar_finish( tar_writer );
    free( tar_writer->actual_tar_file );
    free( tar_writer->cubename );
    free( tar_writer->username );
    free( tar_writer->mode );
    free( tar_writer->group );
    fclose( tar_writer->tar );
    free( tar_writer );
    tar_writer = NULL;
    return tar_writer;
}


void
__cube_set_size_and_calculate_checksum( tar_gnu_header* header, uint64_t size )
{
    /*  set checksum fiels on "         "*/
    sprintf( header->size, "%11.11lo", ( unsigned long )size );
    char* pre_checksum = "        ";
    memcpy( header->checksum, pre_checksum,  strlen( pre_checksum ) );

    uint8_t*      pos      = ( uint8_t* )header;
    unsigned      i        = 0;
    unsigned long checksum = 0;
    for ( i = 0; i < ( sizeof( tar_gnu_header ) ); i++ )
    {
        checksum += ( unsigned long long )( *pos );
        pos++;
    }

    sprintf( header->checksum, "%6.6lo", ( unsigned long )checksum );
}



char*
__cube_get_tared_cube_name( char* cubename )
{
    char* buff = ( char* )calloc( 1, strlen( cubename ) + 7 );
    strcat( buff, cubename );
    strcat( buff, ".cubex" );
    return buff;
}


tar_gnu_header*
__cube_create_tar_header( report_layout_writer* tar_writer, char* filename )
{
    tar_gnu_header* tar_header = ( tar_gnu_header* )calloc( 1, sizeof( tar_gnu_header ) );
    memcpy( tar_header->name, filename,  strlen( filename ) );
    memcpy( tar_header->mode, tar_writer->mode,  strlen( tar_writer->mode ) );
    sprintf( tar_header->uid, "%7.7lo", ( unsigned long )( tar_writer->uid ) );
    sprintf( tar_header->gid, "%7.7lo", ( unsigned long )( tar_writer->gid ) );
    unsigned int mtime = time( NULL );
    sprintf( tar_header->mtime, "%11.11lo", ( unsigned long )mtime );
    memcpy( tar_header->typeflag, "0", 1 );
    memcpy( tar_header->uname, tar_writer->username, strlen( tar_writer->username ) );
    memcpy( tar_header->gname, tar_writer->group, strlen( tar_writer->group ) );

    char* _magic = "ustar";
    memcpy( tar_header->magic, _magic, strlen( _magic ) );
    char* _version = "00";
    memcpy( tar_header->version, _version, strlen( _version ) );

    return tar_header;
}


void
__cube_tar_file_finish( report_layout_writer* tar_writer, uint64_t size )
{
    fflush( tar_writer->tar );
    uint64_t actual_pos = ftell( tar_writer->tar );

    cubew_fseeko( tar_writer->tar, tar_writer->header_position, SEEK_SET );
    __cube_set_size_and_calculate_checksum( tar_writer->actual_tar_header, size );
    fwrite( tar_writer->actual_tar_header, 1, sizeof( tar_gnu_header ), tar_writer->tar );
    cubew_fseeko( tar_writer->tar, actual_pos, SEEK_SET );
    uint64_t difference = ( ( size / sizeof( tar_empty_block ) + 1 ) * sizeof( tar_empty_block )  - size );
    char*    _tmp       = ( char* )calloc( difference, sizeof( char ) );
    fwrite( _tmp, 1, difference, tar_writer->tar );
    free( _tmp );
    free( tar_writer->actual_tar_header );
    tar_writer->actual_tar_header = NULL;
}

#endif
