##
## $Id: Makefile.am 7416 2010-04-16 14:15:36Z mcoutinho $
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

# Descend into the @RTEMS_BSP_FAMILY@ directory
SUBDIRS = @RTEMS_BSP_FAMILY@

EXTRA_DIST = shared/include/sharedSPARC.h

EXTRA_DIST += shared/bspstart.c
EXTRA_DIST += shared/start.S

# AMBA Plug&Play bus
EXTRA_DIST += shared/include/ambapp.h
EXTRA_DIST += shared/amba/ambapp.c

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
