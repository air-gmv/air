##
## $Id: Makefile.am,v 1.31 2007/06/20 22:19:38 joel Exp $
##

include $(top_srcdir)/automake/compile.am

GCOV_FLAGS =
if RTEMS_TEST_HAS_COVERAGE
GCOV_FLAGS += -O0 -fprofile-arcs -ftest-coverage
else
GCOV_FLAGS += -O2
endif

EXEEXT =

project_lib_LIBRARIES = 

project_lib_LIBRARIES += libno_ext.a
libno_ext_a_SOURCES = sapi/no-ext.c
libno_ext_a_CPPFLAGS =  $(AM_CPPFLAGS) $(GCOV_FLAGS)


project_lib_LIBRARIES += libno_io.a
libno_io_a_SOURCES = sapi/no-io.c
libno_io_a_CPPFLAGS =  $(AM_CPPFLAGS) $(GCOV_FLAGS)


project_lib_LIBRARIES += libno_event.a
libno_event_a_SOURCES = rtems/no-event.c
libno_event_a_CPPFLAGS =  $(AM_CPPFLAGS) $(GCOV_FLAGS)


project_lib_LIBRARIES += libno_msg.a
libno_msg_a_SOURCES = rtems/no-msg.c
libno_msg_a_CPPFLAGS =  $(AM_CPPFLAGS) $(GCOV_FLAGS)


project_lib_LIBRARIES += libno_rtmon.a
libno_rtmon_a_SOURCES = rtems/no-rtmon.c
libno_rtmon_a_CPPFLAGS =  $(AM_CPPFLAGS) $(GCOV_FLAGS)


project_lib_LIBRARIES += libno_sem.a
libno_sem_a_SOURCES = rtems/no-sem.c
libno_sem_a_CPPFLAGS =  $(AM_CPPFLAGS) $(GCOV_FLAGS)


project_lib_LIBRARIES += libno_timer.a
libno_timer_a_SOURCES = rtems/no-timer.c
libno_timer_a_CPPFLAGS =  $(AM_CPPFLAGS) $(GCOV_FLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
