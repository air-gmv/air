##
## $Id: Makefile.am 4486 2009-09-23 08:18:02Z jsousa $
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/confdefs.h
include_rtems_HEADERS += include/rtems/config.h include/rtems/fatal.h include/rtems/init.h include/rtems/io.h include/rtems/extension.h \
    include/rtems/sptables.h

EXTRA_DIST = include/rtems/README

include_rtems_HEADERS += inline/rtems/extension.inl

GCOV_FLAGS =
if RTEMS_TEST_HAS_COVERAGE
GCOV_FLAGS += -O0 -fprofile-arcs -ftest-coverage
else
GCOV_FLAGS += -O2
endif

## src
AM_CPPFLAGS += -D__RTEMS_INSIDE__ $(GCOV_FLAGS)

noinst_LIBRARIES = libsapi.a libext.a libio.a

libsapi_a_SOURCES = src/fatal.c src/exinit.c \
    src/exshutdown.c src/rtemsapi.c src/extension.c src/io.c src/ioinitialize.c

libsapi_a_CPPFLAGS = $(AM_CPPFLAGS)

libext_a_SOURCES = src/extensioncreate.c \
    src/extensiondelete.c src/extensionident.c
libext_a_CPPFLAGS = $(AM_CPPFLAGS)

libio_a_SOURCES = src/ioclose.c src/iocontrol.c \
    src/ioopen.c src/ioread.c \
    src/iowrite.c 
libio_a_CPPFLAGS = $(AM_CPPFLAGS)


include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
