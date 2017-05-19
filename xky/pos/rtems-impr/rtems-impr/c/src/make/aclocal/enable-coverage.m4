dnl $Id: enable-coverage.m4 179 2009-01-16 15:11:00 mcoutinho $

AC_DEFUN([RTEMS_ENABLE_COVERAGE],
[
AC_ARG_ENABLE(coverage,
[AC_HELP_STRING([--enable-coverage],[enable coverage (with GCOV) ] )],
  [case "${enableval}" in
    yes) coverage=yes;;
    no)  coverage=no;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-coverage]) ;;
  esac], [coverage=no])
AM_CONDITIONAL([RTEMS_TEST_HAS_COVERAGE],[test x$coverage = xyes])
])
