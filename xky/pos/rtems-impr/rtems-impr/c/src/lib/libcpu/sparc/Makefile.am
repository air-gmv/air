##
## $Id: Makefile.am 8702 2010-08-18 10:23:50Z mcoutinho $
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

GCOV_FLAGS =
if RTEMS_TEST_HAS_COVERAGE
GCOV_FLAGS += -O0 -fprofile-arcs -ftest-coverage
else
GCOV_FLAGS += -O2
endif

include $(top_srcdir)/../../../automake/compile.am

noinst_PROGRAMS =

include_libcpudir = $(includedir)/libcpu
include_libcpu_HEADERS = $(top_srcdir)/../shared/include/cache.h $(top_srcdir)/../sparc/cache/include/cache_.h $(top_srcdir)/../shared/include/sharedCPUs.h $(top_srcdir)/../sparc/include/sharedSPARCCPUs.h $(top_srcdir)/../sparc/reg_win/window.h $(top_srcdir)/../sparc/syscall/syscall.h

noinst_PROGRAMS += syscall.rel
syscall_rel_SOURCES = syscall/syscall.S syscall/syscall.h
syscall_rel_CPPFLAGS = $(AM_CPPFLAGS) $(GCOV_FLAGS)
syscall_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += reg_win.rel
reg_win_rel_SOURCES = reg_win/window.S reg_win/window.h
reg_win_rel_CPPFLAGS = $(AM_CPPFLAGS) $(GCOV_FLAGS)
reg_win_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
