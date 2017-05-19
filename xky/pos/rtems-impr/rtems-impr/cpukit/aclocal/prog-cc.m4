dnl
dnl $Id: prog-cc.m4 179 2008-09-17 14:07:38Z hsilva $
dnl 
dnl Check for target gcc
dnl

AC_DEFUN([RTEMS_PROG_CC],
[
AC_BEFORE([$0], [AC_PROG_CPP])dnl
AC_BEFORE([$0], [AC_PROG_CC])dnl
AC_BEFORE([$0], [RTEMS_CANONICALIZE_TOOLS])dnl

RTEMS_CHECK_TOOL(CC,gcc)
test -z "$CC" && \
  AC_MSG_ERROR([no acceptable cc found in \$PATH])
AC_PROG_CC
AC_PROG_CPP
])

AC_DEFUN([RTEMS_PROG_CC_FOR_TARGET],
[
dnl check target cc
RTEMS_PROG_CC
dnl check if the target compiler may use --pipe
RTEMS_GCC_PIPE
test "$rtems_cv_gcc_pipe" = "yes" && CC="$CC --pipe"

if test "$GCC" = yes; then
RTEMS_CFLAGS="$RTEMS_CFLAGS -Wall"
m4_if([$1],,[],[RTEMS_CFLAGS="$RTEMS_CFLAGS $1"])
fi
AC_SUBST(RTEMS_CFLAGS)

RTEMS_CPPFLAGS="-I\$(top_builddir) -I\$(PROJECT_INCLUDE)"
AC_SUBST(RTEMS_CPPFLAGS)
])
