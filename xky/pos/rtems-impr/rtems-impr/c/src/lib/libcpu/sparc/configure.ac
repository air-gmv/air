## Process this file with autoconf to produce a configure script.
## 
## $Id: configure.ac 7034 2010-04-08 17:02:28Z mcoutinho $

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libcpu-sparc],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([reg_win])
RTEMS_TOP([../../../../..],[../../..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign subdir-objects 1.10])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
AM_PROG_CC_C_O
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

AC_ARG_ENABLE(coverage,
[AC_HELP_STRING([--enable-coverage],[enable coverage (with GCOV) ] )],
  [case "${enableval}" in
    yes) coverage=yes;;
    no)  coverage=no;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-coverage]) ;;
  esac], [coverage=no])
AM_CONDITIONAL([RTEMS_TEST_HAS_COVERAGE],[test x$coverage = xyes])

RTEMS_AMPOLISH3

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

