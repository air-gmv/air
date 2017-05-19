##time estimation
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

nodist_include_HEADERS = 
DISTCLEANFILES = 

noinst_PROGRAMS =

SUBDIRS = . tools

include_HEADERS += include/erc32.h
#include_HEADERS += include/coverhd.h

if BSP_IS_SIS
  CPPASCOMPILE += -DENABLE_SIS_QUIRKS 
endif

EXTRA_DIST = ../../sparc/shared/start.S
start.$(OBJEXT): ../../sparc/shared/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

# erc32 does NOT have cache
noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = clock/ckinit.c \
                   ../../shared/clockdrv_shell.c \
                   ../../shared/bsppost.c \
                   ../../shared/bootcard.c \
                   ../../sparc/shared/bspstart.c \
                   startup/setvec.c \
                   startup/spurious.c \
                   startup/erc32mec.c 
libbsp_a_LIBADD =  
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS) $(GCOV_FLAGS)

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/reg_win.rel \
                   ../../../libcpu/@RTEMS_CPU@/syscall.rel

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
