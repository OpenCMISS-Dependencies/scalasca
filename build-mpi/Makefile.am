## -*- mode: makefile -*-
##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2015                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  Copyright (c) 2009-2013                                                ##
##  German Research School for Simulation Sciences GmbH,                   ##
##  Laboratory for Parallel Programming                                    ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../vendor/common/build-config/m4 -I ../vendor/gtest/m4

## Tell YACC to generate header files
AM_YFLAGS = -d

## Convenience variables
BUILD_DIR=build-backend
bindir=@bindir@@backend_suffix@
libdir=@libdir@@backend_suffix@


## Include Makefile.am snippets
include ../build-includes/common.am
include ../vendor/common/build-config/Makefile.tests.am
include ../build-includes/mpi-backend-only.am

if CROSS_BUILD

AM_CPPFLAGS += -DCROSS_BUILD

else !CROSS_BUILD

AM_CPPFLAGS += -DNOCROSS_BUILD

endif !CROSS_BUILD

AM_CPPFLAGS += -DBACKEND_BUILD_MPI


## Package-specific test setup
if BACKEND_TEST_RUNS

COMPUTE_MPI_LOG_DRIVER = \
    env AM_TAP_AWK='$(AWK)' $(SHELL) $(top_srcdir)/../build-config/tap-driver.sh
COMPUTE_MPI_LOG_DRIVER_FLAGS='--ignore-exit'

else !BACKEND_TEST_RUNS

COMPUTE_MPI_LOG_DRIVER = $(SHELL) \
	$(top_srcdir)/../build-config/test-driver

endif !BACKEND_TEST_RUNS

check-local: skipped_tests
