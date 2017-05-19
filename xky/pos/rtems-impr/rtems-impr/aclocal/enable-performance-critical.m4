dnl $Id: enable-performance.m4 179 2009-01-16 15:11:00 mcoutinho $

AC_DEFUN([RTEMS_ENABLE_PERFORMANCE_CRITICAL],
[
AC_ARG_ENABLE(performance-critical,
[AC_HELP_STRING([--enable-performance-critical],[enable performance tests (tests must be enabled)])],
  [case "${enableval}" in
    yes) performance_critical=yes;;
    no)  performance_critical=no;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-performance-critical ]) ;;
  esac], [performance_critical=no])
AM_CONDITIONAL([RTEMS_TEST_HAS_PERFORMANCE_CRITICAL],[test x$performance_critical = xyes])

AM_CONDITIONAL([RTEMS_TEST_HAS_PERFORMANCE],[test x$performance_critical = xyes])
]) 

