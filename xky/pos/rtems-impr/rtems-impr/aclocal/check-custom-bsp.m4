dnl $Id: check-custom-bsp.m4 179 2008-09-17 14:07:38Z hsilva $

AC_DEFUN([_RTEMS_CHECK_CUSTOM_BSP],[
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])dnl sets RTEMS_CPU, target
AC_REQUIRE([RTEMS_TOP])dnl sets RTEMS_TOPdir
  $2=
  AC_MSG_CHECKING([for $1])
  for i in "${srcdir}/${RTEMS_TOPdir}/bspkit/${RTEMS_CPU}"/*/cfg/"$1" \
    "${srcdir}/${RTEMS_TOPdir}/make/custom/$1";
  do
    AS_IF([test -r $i],[
      $2="$i"
      break;
    ])
  done
  AS_IF([test -n "[$]$2"],
    [AC_MSG_RESULT([[$]$2])],
    [AC_MSG_RESULT([no])])
])

AC_DEFUN([RTEMS_CHECK_CUSTOM_BSP],[
  AC_REQUIRE([RTEMS_TOP])
  _RTEMS_CHECK_CUSTOM_BSP([[$]$1.cfg],[BSP_FOUND])
])
