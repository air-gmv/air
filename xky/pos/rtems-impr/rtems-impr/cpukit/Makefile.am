##
## $Id: Makefile.am 9620 2011-02-23 19:07:52Z mcoutinho $
##

ACLOCAL_AMFLAGS = -I aclocal

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/multilib.am

# librtemscpu
SUBDIRS = . score rtems sapi
SUBDIRS += wrapup

GCOV_FLAGS =
if RTEMS_TEST_HAS_COVERAGE
GCOV_FLAGS += -O0 -fprofile-arcs -ftest-coverage
else
GCOV_FLAGS += -O2
endif

noinst_DATA = preinstall-stamp
preinstall-stamp:
	$(MAKE) $(AM_MAKEFLAGS) preinstall
	touch preinstall-stamp
CLEANFILES = preinstall-stamp

include_dir = $(includedir)
include_HEADERS = include/stddef.h include/stdint.h include/string.h include/time.h

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = include/rtems/concat.h 

include_rtems_rtemsdir = $(includedir)/rtems/rtems
include_rtems_rtems_HEADERS =

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
