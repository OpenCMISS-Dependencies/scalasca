dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2014,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2015,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Technische Universitaet Muenchen, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.69])
AC_INIT([OTF2], m4_esyscmd([vendor/common/build-config/generate-package-version.sh build-config/VERSION]), [support@score-p.org], [otf2])

AFS_PACKAGE_INIT
AFS_SUMMARY_INIT

AC_SCOREP_REVISION

AC_SCOREP_TOPLEVEL_ARGS

## Packages that do manual configuration or use the install program might need
## to tell configure where to find some other shell scripts by calling
## AC_CONFIG_AUX_DIR, though the default places it looks are correct for most
## cases.
## Need to be called early on configure.ac because other macros search tools
## in this directory.
AC_CONFIG_AUX_DIR([build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([build-config/m4 vendor/common/build-config/m4])

## Set the default installation prefix to /opt/otf2 instead of /usr/local
AC_PREFIX_DEFAULT([/opt/otf2])

AC_SCOREP_SVN_CONTROLLED

## Call not before AC_CONFIG_MACRO_DIR
AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax nostdinc])
AM_SILENT_RULES([yes])

AC_SCOREP_DETECT_PLATFORMS
AC_SCOREP_PLATFORM_SETTINGS
AFS_CROSSCOMPILING

# don't detect and load defaults in nested configures (e.g. otf2)
AS_IF([test "x$ac_scorep_platform_data_provided" = "x" ||
       test "${ac_scorep_platform_data_provided}" = "no"],
    [AFS_CUSTOM_COMPILERS
     AS_IF([test "x${afs_custom_compilers_given}" = "xno"],
        [AC_SCOREP_WITH_COMPILER_SUITE])
     # determine arguments for subdir configures
     args="`$AWK -f $srcdir/vendor/common/build-config/process_arguments.awk \
            $ac_scorep_compilers_frontend                                    \
            $ac_scorep_compilers_backend                                     \
            user_provided_configure_args`"
     AS_IF([test $? -ne 0], [AC_MSG_ERROR([cannot process provided and/or autodetected arguments. Please contact <AC_PACKAGE_BUGREPORT> and provide the above output. Thanks.])])
    ],
    [AC_MSG_NOTICE([platform data provided by toplevel configure.])
     # determine arguments for subdir configures
     args="`$AWK -f $srcdir/vendor/common/build-config/process_arguments.awk \
            user_provided_configure_args`"
     AS_IF([test $? -ne 0], [AC_MSG_ERROR([cannot process provided arguments. Please contact <AC_PACKAGE_BUGREPORT> and provide the above output. Thanks.])])
    ])

AC_SCOREP_UNCRUSTIFY

AC_SCOREP_DOXYGEN
AS_IF([test "x${ac_scorep_svn_controlled}" = xyes],
    [AC_CONFIG_FILES([doc/doxygen-user.cfg:doc/doxygen-user/doxygen-user.cfg.in])
     AC_CONFIG_FILES([doc/doxygen-dev.cfg:doc/doxygen-dev/doxygen-dev.cfg.in])])

AFS_DEBUG_OPTION

## Autoconf supports changing the names of programs when installing them. In
## order to use these transformations, configure.ac must call the macro
## AC_ARG_PROGRAM.
# implicitly called by ?
#AC_ARG_PROGRAM

# used for EXTRA_DIST in Makefile.am
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])

# handle own package's subdir configures
AC_CONFIG_SUBDIR_CUSTOM([build-backend], [${args} ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling}])

if test "x${ac_scorep_cross_compiling}" = "xyes"; then
    AC_CONFIG_SUBDIR_CUSTOM([build-frontend], [${args} ac_scorep_platform=${ac_scorep_platform}])
fi

AC_ARG_VAR([PYTHON],
           [The python interpreter to use. Not a build requirement, only
           needed when developing. Python version 2.5 or above, but no support
           for python 3. Use PYTHON=: to disable python support.])
m4_define_default([_AM_PYTHON_INTERPRETER_LIST],
                  [python2 python python2.5])
AM_PATH_PYTHON([2.5],
               [AS_CASE([$PYTHON_VERSION],
                        [2.*], [: accept],
                        [AC_MSG_ERROR([OTF2 does require a python 2 version: $PYTHON_VERSION])])],
               [:])
AM_CONDITIONAL([HAVE_PYTHON], [test "$PYTHON" != :])

AC_CONFIG_FILES([
    Makefile
    src/config-common.h:vendor/common/build-config/config-common.h
    share/otf2/__version__.py
    doc/doxygen-user/main.dox
])
AC_CONFIG_FILES(
    [src/tools/otf2_template/otf2-template:src/tools/otf2_template/otf2_template.py],
    [chmod +x src/tools/otf2_template/otf2-template])

# generate the OTF2 error codes header
AC_CONFIG_FILES(
    [include/otf2/OTF2_ErrorCodes.h:vendor/common/src/utils/exception/ErrorCodes.tmpl.h],
    [$srcdir/vendor/common/src/utils/exception/finalize_error_codes.sh OTF2 $srcdir/share/otf2/otf2.errors include/otf2/OTF2_ErrorCodes.h src/otf2_error_decls.gen.h])

# generate mini-config
adl_RECURSIVE_EVAL([${libexecdir}/${PACKAGE}], [PKGLIBEXECDIR])
AC_SUBST([pkglibexecdir], [${PKGLIBEXECDIR}])
AC_CONFIG_FILES(
    [otf2-mini-config.sh:otf2-mini-config.sh.in],
    [chmod +x otf2-mini-config.sh])

AC_OUTPUT

AFS_SUMMARY_COLLECT([test "x$ac_scorep_platform_data_provided" != "xyes"])
