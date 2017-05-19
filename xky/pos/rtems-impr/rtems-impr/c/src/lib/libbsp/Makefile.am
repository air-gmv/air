##
## $Id: Makefile.am 5446 2009-11-06 18:02:12Z mcoutinho $
##

include $(top_srcdir)/automake/compile.am

SUBDIRS = @libbsp_cpu_subdir@
DIST_SUBDIRS = @libbsp_cpu_subdir@

EXTRA_DIST = MERGE.PROCEDURE bsp.am

# shared
EXTRA_DIST += shared/bootcard.c \
    shared/bsppost.c shared/clockdrv_shell.c

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
