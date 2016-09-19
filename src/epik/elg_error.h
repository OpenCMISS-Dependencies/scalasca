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
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef _ELG_ERROR_H
#define _ELG_ERROR_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Error messages
 * 
 *-----------------------------------------------------------------------------
 */

#define CHK_NOT_NULL(ptr) if (ptr == NULL) { elg_error(); }

/* set process id/rank for messages */
EXTERN void elg_error_pid(const int pid);

/* set thread id function for messages */
EXTERN void elg_error_thread_init(int (*id_fn)(void));  /* supply omp_get_thread_num() as argument */

/* abort and system error message */
#define elg_error() elg_error_impl(__FILE__, __LINE__)
EXTERN void elg_error_impl(const char* f, int l);                          

/* abort and user error message */
EXTERN void elg_error_msg(const char* fmt, ...);

/* user warning message without abort */
EXTERN void elg_warning(const char* fmt, ...);

/* user control message without abort (printed only if ELG_VERBOSE is set) */
EXTERN void elg_cntl_msg(const char* fmt, ...);

#endif






