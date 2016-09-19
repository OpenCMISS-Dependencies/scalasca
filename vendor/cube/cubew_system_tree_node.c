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
 * \file system_tree_node.c
 * \brief Defines types and functions to deal with running system_tree_node as whole object.
 */
#include <config.h>
#include <stdlib.h>
#include <string.h>

#include "cubew_system_tree_node.h"
#include "cubew_location_group.h"
#include "cubew_vector.h"
#include "cubew_services.h"
#include "cubew_meta_data_writer.h"

/**
 * Allocates memory for system_tree_node.
 */
cube_system_tree_node*
cube_system_tree_node_create( cube_system_tree_node* this )
{
    if ( this == NULL )
    {
        ALLOC( this, 1, cube_system_tree_node );
    }
    if ( this != NULL )
    {
        cube_system_tree_node_construct_child( this );
    }
    this->attr = NULL;
    /* construct attrs array */
    XALLOC( this->attr, 1, att_array );
    this->attr->size     = 0;
    this->attr->capacity = 0;

    return this;
}

/**
 * Sets a general desription of the system_tree_node and its name.
 */
void
cube_system_tree_node_init( cube_system_tree_node* this,
                            const char*            name,
                            const char*            desc,
                            const char*            stn_class,
                            cube_system_tree_node* parent
                            )
{
    this->name      = cubew_strdup( name );
    this->desc      = cubew_strdup( desc );
    this->stn_class = cubew_strdup( stn_class );
    this->parent    = parent;
    if ( parent != NULL )
    {
        cube_system_tree_node_add_child( this->parent, this );
    }
}

/**
 * Allocates memory for a child system_tree_node.
 */
void
cube_system_tree_node_construct_child( cube_system_tree_node* this )
{
    XALLOC( this->child, 1, cube_stnarray );
    this->child->size     = 0;
    this->child->capacity = 0;
    XALLOC( this->group, 1, cube_lgarray );
    this->group->size     = 0;
    this->group->capacity = 0;
}

/**
 * Allocates memory for a nodes of the  system_tree_node.
 */
void
cube_system_tree_node_reserve_nodes( cube_system_tree_node* this,
                                     unsigned               num )
{
    this->child->capacity = num;
    ALLOC( this->child->data, this->child->capacity, cube_system_tree_node* );
}

/**
 * Releases memory  of the  system_tree_node.
 */
void
cube_system_tree_node_free( cube_system_tree_node* this )
{
    if ( this != NULL )
    {
        free( this->name );
        free( this->desc );
        free( this->stn_class );
        if ( this->child != NULL )
        {
            free( this->child->data );
        }
        if ( this->group != NULL )
        {
            free( this->group->data );
        }
        free( this->group );
        free( this->child );
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
        free( this );
    }
}



/**
 * Adds an attribute  "m" into cube "this"
 */
void
cube_system_tree_node_add_attr( cube_system_tree_node* this,
                                cmap*                  m )
{
    ADD_NEXT( this->attr, m, cmap* );
}

/**
 * Creates and add in to cube "this" the attribute "key" with a vaue "value"
 */
void
cube_system_tree_node_def_attr( cube_system_tree_node* this,
                                const char*            key,
                                const char*            value )
{
    char* nkey   = cubew_strdup( key );
    char* nvalue = cubew_strdup( value );
    cmap*
    XALLOC( m,
            1,
            cmap );

    m->key   = nkey;
    m->value = nvalue;
    cube_system_tree_node_add_attr( this, m );
}


/**
 * Returns a i-th child on the system_tree_node.
 */
cube_system_tree_node*
cube_system_tree_node_get_child( cube_system_tree_node* this,
                                 int                    i )
{
    if ( i < 0 || i >= this->child->size )
    {
        printf( "cube_system_tree_node_get_child: out of range\n" );
    }
    return this->child->data[ i ];
}

struct cube_location_group*
cube_system_tree_node_get_location_group( cube_system_tree_node* this,
                                          int                    i )
{
    if ( i < 0 || i >= this->group->size )
    {
        printf( "cube_system_tree_node_get_location_group: out of range\n" );
    }
    return this->group->data[ i ];
}

char*
cube_system_tree_node_get_name( cube_system_tree_node* this )
{
    return this->name;
}

char*
cube_system_tree_node_get_desc( cube_system_tree_node* this )
{
    return this->desc;
}
char*
cube_system_tree_node_get_class( cube_system_tree_node* this )
{
    return this->stn_class;
}
int
cube_system_tree_node_num_children( cube_system_tree_node* this )
{
    return this->child->size;
}
int
cube_system_tree_node_num_location_group( cube_system_tree_node* this )
{
    return this->group->size;
}

int
cube_system_tree_node_get_level( cube_system_tree_node* this )
{
    if ( this == NULL )
    {
        fprintf( stderr, "WARNING: argument in the call \"cube_system_tree_node_get_level\" is NULL" );
    }
    int                    level   = 0;
    cube_system_tree_node* _parent = this->parent;
    while ( _parent != NULL )
    {
        level   = level + 1;
        _parent = _parent->parent;
    }
    return level;
}

/**
 * Writes a XML output of the system_tree_node in the .cube file.
 */
void
cube_system_tree_node_writeXML( cube_system_tree_node* this,
                                cube_meta_data_writer* meta_data_writer )
{
    char* _name  = ( char* )__cube_services_escape_to_xml( cube_system_tree_node_get_name( this ) );
    char* _descr = ( char* )__cube_services_escape_to_xml( cube_system_tree_node_get_desc( this ) );
    char* _class = ( char* )__cube_services_escape_to_xml( cube_system_tree_node_get_class( this ) );

    int  i = 0, num = cube_system_tree_node_get_level( this );
    char ind[ 80 ];

    strcpy( ind, "" );
    for ( i = 0; i < 2 * num; i++ )
    {
        strcat( ind, " " );
    }

    __cube_write_meta_data( meta_data_writer,  "%s    <systemtreenode Id=\"%d\">\n", ind, cube_system_tree_node_get_id( this ) );
    __cube_write_meta_data( meta_data_writer,  "%s      <name>%s</name>\n", ind, _name ? _name :  cube_system_tree_node_get_name( this ) );
    __cube_write_meta_data( meta_data_writer,  "%s      <class>%s</class>\n", ind, _class ? _class :  cube_system_tree_node_get_class( this ) );
    if ( strcmp( cube_system_tree_node_get_desc( this ), "" ) != 0 )
    {
        __cube_write_meta_data( meta_data_writer,  "%s      <descr>%s</descr>\n", ind,  _descr ? _descr : cube_system_tree_node_get_desc( this ) );
    }

    __cube_services_write_attributes( meta_data_writer,  this->attr, "      ", ind );


    for ( i = 0; i < cube_system_tree_node_num_location_group( this ); i++ )
    {
        cube_location_group* lg = cube_system_tree_node_get_location_group( this, i );
        cube_location_group_writeXML( lg, meta_data_writer );
    }
    for ( i = 0; i < cube_system_tree_node_num_children( this ); i++ )
    {
        cube_system_tree_node* node = cube_system_tree_node_get_child( this, i );
        cube_system_tree_node_writeXML( node, meta_data_writer );
    }
    __cube_write_meta_data( meta_data_writer,  "%s    </systemtreenode>\n", ind );

    if ( _name )
    {
        free( _name );
    }
    if ( _descr )
    {
        free( _descr );
    }
    if ( _class )
    {
        free( _class );
    }
}

void
cube_system_tree_node_set_id( cube_system_tree_node* this,
                              int                    new_id )
{
    this->id = new_id;
}

int
cube_system_tree_node_get_id( cube_system_tree_node* this )
{
    return this->id;
}

/**
 * Compares eaquality of two system_tree_nodes
 */
int
cube_system_tree_node_equal( cube_system_tree_node* a,
                             cube_system_tree_node* b )
{
    const char* _a = cube_system_tree_node_get_name( a );
    const char* _b = cube_system_tree_node_get_name( b );
    if ( strcmp( _a, _b ) == 0 )
    {
        return 1;
    }
    return 0;
}

/**
 * Ads a location_group "proc" to the location_group "this".
 *
 */
void
cube_system_tree_node_add_child( cube_system_tree_node* parent,
                                 cube_system_tree_node* child )
{
    ADD_NEXT( parent->child, child, cube_system_tree_node* );
}
/**
 * Ads a location_group "proc" to the location_group "this".
 *
 */
void
cube_system_tree_node_add_group( cube_system_tree_node* parent,
                                 cube_location_group*   lg )
{
    ADD_NEXT( parent->group, lg, cube_location_group* );
}
