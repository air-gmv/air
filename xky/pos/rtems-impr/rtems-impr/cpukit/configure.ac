## Process this file with autoconf to produce a configure script.
## 
## $Id: configure.ac 9620 2011-02-23 19:07:52Z mcoutinho $

AC_PREREQ(2.61)
AC_INIT([rtems-cpukit],[_RTEMS_VERSION],[http://rtemscentre.edisoft.pt/bugzilla/])
AC_CONFIG_SRCDIR([score])
RTEMS_TOP([..],[])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define nostdinc subdir-objects foreign 1.10])
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTILIB
RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENABLE_RTEMS_DEBUG
RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_COVERAGE

RTEMS_ENV_RTEMSCPU
RTEMS_CHECK_RTEMS_DEBUG

# Is this a supported CPU?
AC_MSG_CHECKING([if cpu $RTEMS_CPU is supported])
if test -d "$srcdir/score/cpu/$RTEMS_CPU"; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_ERROR(no)
fi

RTEMS_PROG_CC_FOR_TARGET([-fasm])
RTEMS_PROG_CCAS
RTEMS_CANONICALIZE_TOOLS
AM_PROG_CC_C_O
AC_PROG_RANLIB

RTEMS_CHECK_MULTIPROCESSING
RTEMS_CHECK_NETWORKING

rtems_major=`echo _RTEMS_VERSION | sed "s/\..*//"`
rtems_minor=`echo _RTEMS_VERSION | sed "s/[[0-9]][[0-9]]*\.//;s/\..*//"`
rtems_revision=`echo _RTEMS_VERSION | sed "s/[[0-9]][[0-9]]*\.//;s/[[0-9]][[0-9]]*\.//;s/\..*//"`

AC_ENABLE_MULTILIB([Makefile],[..])

# ... far too many conditionals ...
AM_CONDITIONAL(LIBRPC,[test x"$rtems_cv_HAS_NETWORKING" = x"yes"])

AM_CONDITIONAL(HAS_MP,test x"$enable_multiprocessing" = x"yes" )

AM_CONDITIONAL(LIBNETWORKING,test x"$rtems_cv_HAS_NETWORKING" = x"yes")

AM_CONDITIONAL([NEED_SYS_CDEFS_H],[test x"$NEED_SYS_CDEFS_H" = x"yes"])
AM_CONDITIONAL([NEED_SYS_QUEUE_H],[test x"$NEED_SYS_QUEUE_H" = x"yes"])

AM_CONDITIONAL([LIBSHELL],[test x"$HAVE_ASSIGNABLE_STDIO" = x"yes"])
AM_CONDITIONAL([LIBSERDBG],[test x"$rtems_cv_cc_attribute_weak" = x"yes"])

RTEMS_AMPOLISH3

# Explicitly list all Makefiles here
AC_CONFIG_FILES([
Makefile
rtems/Makefile
sapi/Makefile
score/Makefile
score/cpu/Makefile
score/cpu/sparc/Makefile
wrapup/Makefile])

AC_CONFIG_COMMANDS([preinstall-stamp],
[test -z "$with_multisubdir" && ${MAKE} preinstall-stamp],
[MAKE=${MAKE}
with_multisubdir="$with_multisubdir"])

AC_OUTPUT
