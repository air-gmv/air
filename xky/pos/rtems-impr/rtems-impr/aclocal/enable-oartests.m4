dnl $Id: enable-tests.m4 179 2008-09-17 14:07:38Z hsilva $

AC_DEFUN([RTEMS_ENABLE_OARTESTS],
[
AC_ARG_ENABLE(oartests,
[AC_HELP_STRING([--enable-oartests],[enable oartests (default:no)])],
  [case "${enableval}" in
    samples) enable_oartests=samples;;
    yes) enable_oartests=yes ;;
    no)  enable_oartests=no ;;
    *)   AC_MSG_ERROR([bad value ${enableval} for --enable-oartests]) ;;
  esac], [enable_oartests=no])
])
