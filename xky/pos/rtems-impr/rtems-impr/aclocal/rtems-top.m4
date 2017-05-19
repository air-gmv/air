dnl $Id: rtems-top.m4 179 2008-09-17 14:07:38Z hsilva $

dnl
dnl RTEMS_TOP($1)
dnl 
dnl $1 .. relative path from this configure.in to the toplevel configure.in
dnl
AC_DEFUN([RTEMS_TOP],
[dnl
AC_REQUIRE([RTEMS_VERSIONING])
AC_CONFIG_AUX_DIR([$1])
AC_CHECK_PROGS(MAKE, gmake make)
AC_BEFORE([$0], [AM_INIT_AUTOMAKE])dnl

AC_PREFIX_DEFAULT([/opt/rtems-][RTEMS_API])

RTEMS_TOPdir="$1";
AC_SUBST(RTEMS_TOPdir)

dots=`echo $with_target_subdir|\
sed -e 's,^\.$,,' -e 's%^\./%%' -e 's%[[^/]]$%&/%' -e 's%[[^/]]*/%../%g'`
PROJECT_TOPdir=${dots}${RTEMS_TOPdir}/'$(top_builddir)'
AC_SUBST(PROJECT_TOPdir)

PROJECT_ROOT="${RTEMS_TOPdir}/\$(top_builddir)"
AC_SUBST(PROJECT_ROOT)

AC_MSG_CHECKING([for RTEMS Version])
AC_MSG_RESULT([_RTEMS_VERSION])
])dnl
