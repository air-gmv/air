dnl $Id: env-rtemscpu.m4 179 2008-09-17 14:07:38Z hsilva $

AC_DEFUN([RTEMS_ENV_RTEMSCPU],
[
  AC_REQUIRE([RTEMS_ENABLE_MULTILIB])

  AM_CONDITIONAL([MULTILIB],[test x"$multilib" = x"yes"])
])
