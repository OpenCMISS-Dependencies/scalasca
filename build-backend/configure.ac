##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2016                                                ##
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


##--- Setup -----------------------------------------------------------------

## Set up Autoconf
AC_PREREQ([2.69])
AC_INIT([Scalasca],
        m4_esyscmd([../vendor/common/build-config/generate-package-version.sh ../build-config/VERSION]),
        [scalasca@fz-juelich.de],,[http://www.scalasca.org])

## Where to find auxiliary build tools & local macros
AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../vendor/common/build-config/m4 ../vendor/gtest/m4])

## Set up Automake
AM_INIT_AUTOMAKE([1.13.4 -Wall foreign parallel-tests color-tests tar-pax])
AM_SILENT_RULES([yes])

## Initialize configuration summary
AFS_PACKAGE_BUILD_INIT([backend])
AFS_SUMMARY_INIT

## Set (non-)cross compilation mode
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
      [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])
cross_compiling=${ac_scorep_cross_compiling}
## Suppress warning about 'using cross tools not prefixed with host triplet'
## when in cross-compile mode
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
    [ac_tool_warned=yes])

## Set up Libtool
AM_PROG_AR
LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

## Set convenience variables for macros
ac_scorep_backend=yes
AM_COND_IF([CROSS_BUILD],
    [ac_scorep_frontend=no],
    [ac_scorep_frontend=yes])

## On crosscompile machines, install backend/mic stuff into
## $(bin|lib)dir/(backend|mic).
backend_suffix=""
AM_COND_IF([CROSS_BUILD],
    [backend_suffix="/backend"
     AS_IF([test "x${ac_scorep_platform}" = "xmic"],
         [backend_suffix="/mic"])
    ])
AC_SUBST(backend_suffix)

## Platform configuration
AC_SCOREP_PLATFORM_SETTINGS
scalasca_platform_includes=""
AM_COND_IF([PLATFORM_BGQ],
           [scalasca_platform_includes="-I/bgsys/drivers/ppcfloor -I/bgsys/drivers/ppcfloor/spi/include/kernel/cnk"])
AC_SUBST([PLATFORM_INCLUDES], ["${scalasca_platform_includes}"])
AM_COND_IF([CROSS_BUILD],
    [scalasca_build_scan="no"
     AS_CASE([${ac_scorep_platform}],
         [k],     [scalasca_build_scan="yes"],
         [fx10],  [scalasca_build_scan="yes"],
         [fx100], [scalasca_build_scan="yes"],
         [mic],   [scalasca_build_scan="yes"])
    ],
    [scalasca_build_scan=yes])
AM_CONDITIONAL([BUILD_SCAN_TOOL], [test "x${scalasca_build_scan}" = "xyes"])
AM_CONDITIONAL([BUILD_SCALASCA_TOOL], [test "x${scalasca_build_scan}" = "xyes"])

## Configure internal/external libraries
AS_IF([test -n "${scorep_otf2_bindir}"],
      [scorep_otf2_config_backend_arg=
       AM_COND_IF([CROSS_BUILD],
           [scorep_otf2_config_backend_arg="--target=backend"
            AS_IF([test "x${ac_scorep_platform}" = "xmic"],
                [scorep_otf2_config_backend_arg="--target=mic"])
           ])
       AC_SUBST([OTF2_CPPFLAGS], ["`${scorep_otf2_bindir}/otf2-config --cppflags ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_LIBS],     ["`${scorep_otf2_bindir}/otf2-config --libs ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_LDFLAGS],  ["`${scorep_otf2_bindir}/otf2-config --ldflags ${scorep_otf2_config_backend_arg}`"])
       AM_CONDITIONAL([HAVE_EXTERNAL_OTF2_LIBRARY], [test 1 -eq 1])],
      [AC_SUBST([OTF2_CPPFLAGS], ['-I../vendor/otf2/include -I$(srcdir)/../vendor/otf2/include'])
       AC_SUBST([OTF2_LIBS],     [])
       AC_SUBST([OTF2_LDFLAGS],  [])
       AM_CONDITIONAL([HAVE_EXTERNAL_OTF2_LIBRARY], [test 1 -eq 0])])
AS_IF([test -n "${scorep_cube_bindir}"],
      [scorep_cube_config_backend_arg=""
       AM_COND_IF([CROSS_BUILD],
           [scorep_cube_config_backend_arg="--target=backend"
            AS_IF([test "x${ac_scorep_platform}" = "xmic"],
                [scorep_cube_config_backend_arg="--target=mic"])
           ])
       AC_SUBST([CUBE_WRITER_CPPFLAGS], ["`${scorep_cube_bindir}/cube-config --writer-cppflags`"])
       AC_SUBST([CUBE_WRITER_LIBS],     ["`${scorep_cube_bindir}/cube-config --writer-libs`"])
       AC_SUBST([CUBE_WRITER_LDFLAGS],  ["`${scorep_cube_bindir}/cube-config --writer-ldflags ${scorep_cube_config_backend_arg}`"])
       AM_CONDITIONAL([HAVE_EXTERNAL_CUBE_WRITER], [test 1 -eq 1])
      ],
      [AC_SUBST([CUBE_WRITER_CPPFLAGS], ['-I$(srcdir)/../vendor/cube -DCUBE_AS_SVN_EXTERNAL'])
       AC_SUBST([CUBE_WRITER_LIBS],     [])
       AC_SUBST([CUBE_WRITER_LDFLAGS],  [])
       AM_CONDITIONAL([HAVE_EXTERNAL_CUBE_WRITER], [test 1 -eq 0])
      ])


##--- Package-specific configuration ----------------------------------------

## Consistency checks for compilers
AC_SCOREP_CHECK_COMPILER_VAR_SET([CC])
AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])

## Checks for platform specifics
SCALASCA_DETECT_MIC_BUILD

## Checks for programs
AC_PROG_CC
AC_SCOREP_COMPILER_CHECKS
AC_SCOREP_PROG_CC_C99
AC_C_BIGENDIAN
AC_PROG_CXX
## Force GCC to C++98 mode, as GCC >= 6.1 defaults to C++14 which breaks the
## build...
AS_IF([test "x${ax_cv_c_compiler_vendor}" = xgnu],
    [CXX="${CXX} -std=c++98"])
AC_LANG([C++])
AM_COND_IF([CROSS_BUILD],
           [], # Handled by frontend configure in cross-compilation mode
           [AM_PROG_LEX
            AC_PROG_YACC])

## Check for OpenMP support
AC_OPENMP
SCALASCA_NOWARN_OMP_PRAGMAS
AM_CONDITIONAL([OPENMP_SUPPORTED],
               [test "x${ac_cv_prog_cxx_openmp}" != "xunsupported" \
                && test "x${enable_openmp}" != "xno"])
AS_IF([test "x${ac_cv_prog_cxx_openmp}" = "xunsupported"],
      [AC_MSG_WARN([no suitable OpenMP compilers found. Scalasca OpenMP support disabled.])])

## Handle auxiliary configure options
SCALASCA_DEBUG_OPTION
AC_SCOREP_BACKEND_TEST_RUNS

## Set PEARL library versioning information
AC_SUBST([PEARL_INTERFACE_VERSION],
         m4_esyscmd([../vendor/common/build-config/generate-library-version.sh ../build-config/VERSION]))

## When building Cube ourselves, we need to include the checks for fseeko in
## the configure, because then Cube uses the Scalasca's config.h
AC_SCOREP_POSIX_FUNCTIONS

## Checks for headers/functions
AC_SCALASCA_GETOPT
AC_SCALASCA_ZLIB
AX_GTEST([../vendor/gtest])


##--- Generate output -------------------------------------------------------

## Backend configuration summary
AFS_SUMMARY([C99 compiler used], [$CC])
AFS_SUMMARY([C++ compiler used], [$CXX])
AS_IF([test "x${enable_openmp}" = "xno"],
      [AFS_SUMMARY([OpenMP support], [disabled])],
      [test "x${ac_cv_prog_cxx_openmp}" = "xunsupported"],
      [AFS_SUMMARY([OpenMP support], [no, no suitable compilers found])],
      [AFS_SUMMARY([OpenMP support], [yes, using ${ac_cv_prog_cxx_openmp}])])

## Generate config files
AC_CONFIG_HEADERS([../src/config-backend.h])
AC_CONFIG_FILES([Makefile], [
    if test "x${ac_scorep_cross_compiling}" = "xyes"; then
        sed -e 's/_0 = @echo "  \(YACC\|GEN\|LEX\)/&  /; t; s/_0 = @echo "  /&BE/' \
        Makefile >Makefile.be && mv -f Makefile.be Makefile || rm -f Makefile.be
    fi
], [ac_scorep_cross_compiling='$ac_scorep_cross_compiling'])
AC_CONFIG_FILES([../src/pearl/example/Makefile.serial])
AM_COND_IF([OPENMP_SUPPORTED],
           [AC_CONFIG_FILES([../src/pearl/example/Makefile.openmp])])

## Workaround for linking issues on BG/Q, Cray, K & FX10/100
AS_IF([test "x${enable_shared}" = xno],
    [AS_CASE([${ac_scorep_platform}],
         [bgq|crayx*|k|fx10*],
             [AC_CONFIG_COMMANDS([libtool-static-fix],
                                 [sed -e '1,/prefer_static_libs=/ s/prefer_static_libs=.*$/prefer_static_libs=yes/' \
                                  libtool > libtool.fix && mv libtool.fix libtool])
             ])
    ])
AC_OUTPUT
