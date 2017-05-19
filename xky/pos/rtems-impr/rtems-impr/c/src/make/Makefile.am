##
## $Id: Makefile.am 4486 2009-09-23 08:18:02Z jsousa $
##

ACLOCAL_AMFLAGS = -I aclocal -I ../aclocal

rtems_makedir = $(prefix)/make

GCOV_FLAGS =
if RTEMS_TEST_HAS_COVERAGE
GCOV_FLAGS += -O0 -fprofile-arcs -ftest-coverage
else
GCOV_FLAGS += -O2
endif

GENERIC_FILES = host.cfg

MAKE_FILES = README directory.cfg lib.cfg

rtems_make_DATA = $(MAKE_FILES) $(GENERIC_FILES)

noinst_DATA = leaf.cfg
EXTRA_DIST = leaf.cfg

rtemsdir = $(exec_prefix)
rtems_bspdir = $(rtemsdir)/@RTEMS_BSP@
rtems_bsp_DATA = Makefile.inc

rtems_bsp_makedir = $(rtems_bspdir)/make
rtems_bsp_make_DATA = bsp.cfg target.cfg

if MAINTAINER_MODE
$(srcdir)/host.cfg.in: $(top_srcdir)/@RTEMS_TOPdir@/make/host.cfg.in
	cp $< $@
endif

## At the moment all actively supported configurations
## use gcc-target-default.cfg only.
rtems_make_compilersdir = $(rtems_makedir)/compilers
dist_rtems_make_compilers_DATA = compilers/gcc-target-default.cfg

include $(top_srcdir)/../automake/local.am
