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
 * \file cubew_report_layouts.c
 * \brief Includes proper combinatins of file layout and name transformations according to the choosen (via macro) report layout.
 *
 *
 *
 */

#ifndef __CUBEW_REPORT_LAYOUTS_C
#define __CUBEW_REPORT_LAYOUTS_C 0

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include <config.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "cubew_cube.h"
#include "cubew_cubew.h"


#ifdef CUBE_LAYOUT_TAR
#ifdef CUBE_LAYOUT_DIRECT
#error Only one layout has to be choosen. Either "tar" via macro CUBE_LAYOUT_TAR or "direct" via macro CUBE_LAYOUT_DIRECT
#endif
#ifndef CUBE_LAYOUT_DIRECT

// #warning "TAR LAYOUT!"

#include "cubew_file_layout_embedded.c"
#include "cubew_tar_writing.c"

#endif
#endif


#ifdef CUBE_LAYOUT_DIRECT

#ifdef CUBE_LAYOUT_TAR
#error Only one layout has to be choosen. Either "tar" via macro CUBE_LAYOUT_TAR or "direct" via macro CUBE_LAYOUT_DIRECT
#endif

#ifndef CUBE_LAYOUT_TAR

// #warning "DIRECT HYBRID LAYOUT!"

#include "cubew_file_layout_hybrid.c"
#include "cubew_direct_writing.c"

#endif

#endif

#ifndef CUBE_LAYOUT_DIRECT
#ifndef CUBE_LAYOUT_TAR
// default includes are
// #warning "DEFUALT LAYOUT! "
#include "cubew_file_layout_embedded.c"
#include "cubew_tar_writing.c"
#endif
#endif

#endif
