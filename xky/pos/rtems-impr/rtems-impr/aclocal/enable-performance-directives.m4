dnl $Id: enable-performance.m4 179 2009-01-16 15:11:00 mcoutinho $

AC_DEFUN([RTEMS_ENABLE_PERFORMANCE_DIRECTIVES],
[
AC_ARG_ENABLE(performance-directives,
[AC_HELP_STRING([--enable-performance-directives],[enable performance tests (tests must be enabled)])],
  [case "${enableval}" in
    yes) performance_directives=yes;;
    no)  performance_directives=no;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-performance-directives]) ;;
  esac], [performance_directives=no])
AM_CONDITIONAL([RTEMS_TEST_HAS_PERFORMANCE_DIRECTIVES],[test x$performance_directives = xyes])

AM_CONDITIONAL([RTEMS_TEST_HAS_PERFORMANCE],[test x$performance_directives = xyes])
]) 

