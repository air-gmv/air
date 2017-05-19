# $Id: rtems-ampolish.m4 179 2008-09-17 14:07:38Z hsilva $

AC_DEFUN([RTEMS_AMPOLISH3],
[
AC_REQUIRE([AM_INIT_AUTOMAKE])
AC_PATH_PROG([PERL],[perl],[])
AC_SUBST(AMPOLISH3,["\$(PERL) ${am_aux_dir}/ampolish3"])
AM_CONDITIONAL([AMPOLISH3],
[test x"$USE_MAINTAINER_MODE" = x"yes" \
  && test -n "${PERL}"])
])

