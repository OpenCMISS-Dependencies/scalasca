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
 * \file location_group.c
   \brief Defines types and functions to deal with location group of running application.
 */
#include <config.h>
#include <stdlib.h>
#include <string.h>
#include "cubew_location_group.h"
#include "cubew_system_tree_node.h"
#include "cubew_location.h"
#include "cubew_meta_data_writer.h"
#include "cubew_vector.h"
#include "cubew_services.h"

/**
 * Allocates memory for a location_group structure.
 *
 */
cube_location_group*
cube_location_group_create( cube_location_group* this )
{
    if ( this == NULL )
    {
        ALLOC( this, 1, cube_location_group );
    }
    if ( this != NULL )
    {
        cube_location_group_construct_child( this );
    }
    this->attr = NULL;
    /* construct attrs array */
    XALLOC( this->attr, 1, att_array );
    this->attr->size     = 0;
    this->attr->capacity = 0;

    return this;
}

/**
 * Fills the structure with name and rank of location_group.
 *
 */
void
cube_location_group_init( cube_location_group*     this,
                          const char*              name,
                          int                      rank,
                          cube_location_group_type type,
                          cube_system_tree_node*   parent )
{
    this->name   = cubew_strdup( name );
    this->rank   = rank;
    this->parent = parent;
    this->type   = type;
    if ( parent != NULL )
    {
        cube_location_group_add_child( this->parent, this );
    }
}

/**
 * Creates a child of this location_group.
 *
 */
void
cube_location_group_construct_child( cube_location_group* this )
{
    XALLOC( this->child, 1, cube_larray );
    this->child->size     = 0;
    this->child->capacity = 0;
}

/**
 * Ads a location_group "proc" to the location_group "this".
 *
 */
void
cube_location_group_add_child( cube_system_tree_node* parent,
                               cube_location_group*   lg )
{
    ADD_NEXT( parent->group, lg, cube_location_group* );
}

/**
 * Releases memory for a location_group structure.
 *
 */
void
cube_location_group_free( cube_location_group* this )
{
    if ( this != NULL )
    {
        if ( this->name != 0 )
        {
            free( this->name );
        }
        if ( this->child != NULL )
        {
            free( this->child->data );
        }
        int i = 0;
        if ( this->attr )
        {
            for ( i = 0; i < this->attr->size; i++ )
            {
                free( ( this->attr->data[ i ] )->key );
                free( ( this->attr->data[ i ] )->value );
                free( this->attr->data[ i ] );
            }
            free( this->attr->data );
            free( this->attr );
        }

        free( this->child );
        free( this );
    }
}



/**
 * Adds an attribute  "m" into cube "this"
 */
void
cube_location_group_add_attr( cube_location_group* this,
                              cmap*                m )
{
    ADD_NEXT( this->attr, m, cmap* );
}

/**
 * Creates and add in to cube "this" the attribute "key" with a vaue "value"
 */
void
cube_location_group_def_attr( cube_location_group* this,
                              const char*          key,
                              const char*          value )
{
    char* nkey   = cubew_strdup( key );
    char* nvalue = cubew_strdup( value );
    cmap*
    XALLOC( m,
            1,
            cmap );

    m->key   = nkey;
    m->value = nvalue;
    cube_location_group_add_attr( this, m );
}


cube_location*
cube_location_group_get_child( cube_location_group* this,
                               int                  i )
{
    if ( i < 0 || i >= this->child->size )
    {
        printf( "cube_location_group_get_child: out of range\n" );
    }
    return this->child->data[ i ];
}

cube_system_tree_node*
cube_location_group_get_parent( cube_location_group* this )
{
    return this->parent;
}

int
cube_location_group_get_rank( cube_location_group* this )
{
    return this->rank;
}

char*
cube_location_group_get_name( cube_location_group* this )
{
    return this->name;
}

cube_location_group_type
cube_location_group_get_type( cube_location_group* this )
{
    return this->type;
}

int
cube_location_group_num_children( cube_location_group* this )
{
    return this->child->size;
}

/**
 * Gets a level of the location_group.
 * Does it recursiv with "deep search" algorithm
 */
int
cube_location_group_get_level( cube_location_group* this )
{
    if ( cube_location_group_get_parent( this ) == NULL )
    {
        return 0;
    }
    else
    {
        return cube_system_tree_node_get_level( cube_location_group_get_parent( this ) ) + 1;
    }
}

/**
 * Writes XML output for location_group in to .cube file.
 * Does it recursiv with "deep search" algorithm
 */
void
cube_location_group_writeXML( cube_location_group*   this,
                              cube_meta_data_writer* meta_data_writer )
{
    char* _name = ( char* )__cube_services_escape_to_xml( cube_location_group_get_name( this ) );

    int  i = 0, num = cube_location_group_get_level( this );
    char ind[ 80 ];

    strcpy( ind, "" );
    for ( i = 0; i < 2 * num; i++ )
    {
        strcat( ind, " " );
    }
    cube_location_group_type __type = cube_location_group_get_type( this );
    char*                    _type  = ( __type == CUBE_LOCATION_GROUP_TYPE_PROCESS ) ? "process" :
                                      ( __type == CUBE_LOCATION_GROUP_TYPE_METRICS ) ? "metrics" :
                                      "not supported";
    __cube_write_meta_data( meta_data_writer,  "%s    <locationgroup Id=\"%d\">\n", ind, cube_location_group_get_id( this ) );
    __cube_write_meta_data( meta_data_writer,  "%s      <name>%s</name>\n", ind, _name ? _name : cube_location_group_get_name( this ) );
    __cube_write_meta_data( meta_data_writer,  "%s      <rank>%d</rank>\n", ind, cube_location_group_get_rank( this ) );
    __cube_write_meta_data( meta_data_writer,  "%s      <type>%s</type>\n", ind, _type );
    __cube_services_write_attributes( meta_data_writer,  this->attr, "      ", ind );
    for ( i = 0; i < cube_location_group_num_children( this ); i++ )
    {
        cube_location* thrd = cube_location_group_get_child( this, i );
        cube_location_writeXML( thrd, meta_data_writer );
    }
    __cube_write_meta_data( meta_data_writer,  "%s    </locationgroup>\n", ind );

    if ( _name )
    {
        free( _name );
    }
}

void
cube_location_group_set_id( cube_location_group* this,
                            int                  new_id )
{
    this->id = new_id;
}

int
cube_location_group_get_id( cube_location_group* this )
{
    return this->id;
}

/**
 * Compares equality of two location_groupes.
 */
int
cube_location_group_equal( cube_location_group* a,
                           cube_location_group* b )
{
    unsigned _a = cube_location_group_get_rank( a );
    unsigned _b = cube_location_group_get_rank( b );
    if ( _a == _b  )
    {
        return 1;
    }
    return 0;
}
