dnl $Id: enable-networking.m4 179 2008-09-17 14:07:38Z hsilva $

AC_DEFUN([RTEMS_ENABLE_NETWORKING],
[
## AC_BEFORE([$0], [RTEMS_CHECK_NETWORKING])dnl

AC_ARG_ENABLE(networking,
[AS_HELP_STRING(--enable-networking,enable TCP/IP stack)],
[case "${enableval}" in
  yes) RTEMS_HAS_NETWORKING=yes ;;
  no) RTEMS_HAS_NETWORKING=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-networking option) ;;
esac],[RTEMS_HAS_NETWORKING=yes])
AC_SUBST(RTEMS_HAS_NETWORKING)dnl
])
