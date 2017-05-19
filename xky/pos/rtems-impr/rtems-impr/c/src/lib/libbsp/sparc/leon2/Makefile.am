##
## $Id: Makefile.am 7634 2010-05-04 16:02:00Z jsousa $
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

GCOV_FLAGS =
if RTEMS_TEST_HAS_COVERAGE
GCOV_FLAGS += -O0 -fprofile-arcs -ftest-coverage
else
GCOV_FLAGS += -O2
endif

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/sharedBSPs.h
include_HEADERS += ../shared/include/sharedSPARC.h


nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools

include_HEADERS += include/leon.h
#include_HEADERS += include/coverhd.h

EXTRA_DIST = ../../sparc/shared/start.S
start.$(OBJEXT): ../../sparc/shared/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = ../../shared/bsppost.c ../../sparc/shared/bspstart.c \
                   ../../shared/bootcard.c startup/setvec.c \
                   ../../shared/clockdrv_shell.c \
                   ../../../libcpu/shared/src/cache_manager.c \
                   ../../../libcpu/sparc/cache/cache.c \
                   startup/spurious.c \
                   clock/ckinit.c
libbsp_a_LIBADD = 
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS) $(GCOV_FLAGS)

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/reg_win.rel \
                   ../../../libcpu/@RTEMS_CPU@/syscall.rel

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
