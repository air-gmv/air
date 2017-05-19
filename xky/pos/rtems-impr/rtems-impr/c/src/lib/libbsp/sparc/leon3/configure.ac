## Process this file with autoconf to produce a configure script.
## 
## $Id: configure.ac 7134 2010-04-09 09:58:12Z mcoutinho $

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-sparc-leon],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING

AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

AC_ARG_ENABLE(coverage,
[AC_HELP_STRING([--enable-coverage],[enable coverage (with GCOV) ] )],
  [case "${enableval}" in
    yes) coverage=yes;;
    no)  coverage=no;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-coverage]) ;;
  esac], [coverage=no])
AM_CONDITIONAL([RTEMS_TEST_HAS_COVERAGE],[test x$coverage = xyes])

RTEMS_CONFIG_BUILD_SUBDIRS(tools)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
