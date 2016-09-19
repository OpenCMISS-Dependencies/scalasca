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
 * \file cubew_meta_dadta_writer.c
   \brief Provides implementation of API for unified meta data writing, if compressed or not.
 */
#ifndef CUBEW_META_DATA_WRITER_C
#define CUBEW_META_DATA_WRITER_C

#define META_DATA_WRITER_BUFFER_CHUNK_SIZE 1024*1024*2 /* 2 Mb for buffer  - t oavoind unnecessary reallocs*/
#include <stdlib.h>
#include "cubew_meta_data_writer.h"

#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
#include <zlib.h>
#endif

cube_meta_data_writer*
create_meta_data_writer( FILE* fh )
{
    cube_meta_data_writer* _writer = ( cube_meta_data_writer* )malloc( sizeof( cube_meta_data_writer ) );
    _writer->file      = fh;
    _writer->buffer    = ( char* )malloc( META_DATA_WRITER_BUFFER_CHUNK_SIZE );
    _writer->bufferlen = META_DATA_WRITER_BUFFER_CHUNK_SIZE;
    _writer->pointer   = _writer->buffer; /* we start to write at the beginning of the buffer */
    return _writer;
}

void
__cube_write_meta_data( cube_meta_data_writer* writer, char* format, ... )
{
    va_list fmtargs;
    va_start( fmtargs, format );

#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )
    int len = 0;
    len = vsnprintf( NULL, 0, format, fmtargs );
    va_end( fmtargs );
    if ( len > writer->bufferlen - ( writer->pointer - writer->buffer ) )
    {   /* not enough buffer to write, so, we realloc for CHUNK + len */
        /* note how far pointer is from the buffer*/
        int offset = writer->pointer - writer->buffer;
        /* realloc */
        writer->buffer = ( char* )realloc( writer->buffer, writer->bufferlen + META_DATA_WRITER_BUFFER_CHUNK_SIZE + len );
        /* restore correct pointer */
        writer->pointer    = writer->buffer + offset;
        writer->bufferlen += META_DATA_WRITER_BUFFER_CHUNK_SIZE + len;
    }
    /* write */
    va_start( fmtargs, format );
    vsprintf( writer->pointer, format, fmtargs );
    writer->pointer += len;
#else
    vfprintf( writer->file, format, fmtargs );
#endif
    va_end( fmtargs );
}

void
close_meta_data_writer( cube_meta_data_writer* writer )
{
#if defined( BACKEND_CUBE_COMPRESSED ) || defined( FRONTEND_CUBE_COMPRESSED )

    /* here we compress the buffer */
    uLongf uncompressed_buffer_size =  writer->pointer - writer->buffer;

    uLongf compressed_buffer_size = compressBound(  uncompressed_buffer_size );
    Bytef* _z_buffer              = ( Bytef* )malloc( compressed_buffer_size );
//     compress( _z_buffer,   &compressed_buffer_size, ( Bytef* )( writer->buffer ),  writer->pointer - writer->buffer  );

    z_stream defstream;
    defstream.zalloc    = Z_NULL;
    defstream.zfree     = Z_NULL;
    defstream.opaque    = Z_NULL;
    defstream.avail_in  = ( uInt )uncompressed_buffer_size;   // size of input
    defstream.next_in   = ( Bytef* )( writer->buffer );       // input char array
    defstream.avail_out = ( uInt )compressed_buffer_size + 1; // size of output
    defstream.next_out  = ( Bytef* )_z_buffer;                // output char array

    deflateInit2( &defstream,
                  Z_DEFAULT_COMPRESSION,
                  Z_DEFLATED,
                  31, /* 10 + 16 */
                  8,  /* default level */
                  Z_DEFAULT_STRATEGY
                  );
    deflate( &defstream, Z_FINISH );
    deflateEnd( &defstream );

    fwrite( _z_buffer, defstream.total_out, 1, writer->file );
    free( _z_buffer );


//         fwrite( writer->buffer, writer->pointer - writer->buffer, 1, writer->file );
#endif
    free( writer->buffer );
    free( writer );
}


#endif
