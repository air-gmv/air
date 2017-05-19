dnl
dnl $Id: canonicalize-tools.m4 179 2008-09-17 14:07:38Z hsilva $
dnl
dnl Set target tools
dnl

AC_DEFUN([RTEMS_CANONICALIZE_TOOLS],
[AC_REQUIRE([RTEMS_PROG_CC])dnl

dnl FIXME: What shall be done if these tools are not available?
  RTEMS_CHECK_TOOL(AR,ar,no)
  RTEMS_CHECK_TOOL(AS,as,no)
  RTEMS_CHECK_TOOL(LD,ld,no)

dnl special treatment of ranlib
  RTEMS_CHECK_TOOL(RANLIB,ranlib,:)
])
