dnl $Id: check-custom-bsp.m4 179 2008-09-17 14:07:38Z hsilva $

AC_DEFUN([_RTEMS_CHECK_CUSTOM_BSP],[
  for i in "${srcdir}/${RTEMS_TOPdir}/bspkit/${RTEMS_CPU}"/*/cfg/$1 \
    ${srcdir}/${RTEMS_TOPdir}/make/custom/$1;
  do
    AC_MSG_CHECKING([for $i])
    AS_IF([test -r $i],[
      $2="$i"
      AC_MSG_RESULT([yes])
      break;
    ],[
      AC_MSG_RESULT([no])
    ])
  done
])

AC_DEFUN([RTEMS_CHECK_CUSTOM_BSP],[
  AC_REQUIRE([RTEMS_TOP])
  _RTEMS_CHECK_CUSTOM_BSP([[$]$1.cfg],[BSP_FOUND])
  AS_IF([test -z "$BSP_FOUND"],[
    AC_MSG_ERROR([missing [$]$1.cfg])
  ])
])
