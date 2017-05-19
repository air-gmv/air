##
## $Id: Makefile.am 4486 2009-09-23 08:18:02Z jsousa $
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

GCOV_FLAGS =
if RTEMS_TEST_HAS_COVERAGE
GCOV_FLAGS += -O0 -fprofile-arcs -ftest-coverage
else
GCOV_FLAGS += -O2
endif

AM_CPPFLAGS += -D__RTEMS_INSIDE__ $(GCOV_FLAGS)

noinst_LIBRARIES = librtems.a libsem.a libmsg.a libevent.a librtmon.a libtimer.a
librtems_a_CPPFLAGS = $(AM_CPPFLAGS)

libsem_a_CPPFLAGS = $(AM_CPPFLAGS)

libmsg_a_CPPFLAGS = $(AM_CPPFLAGS)

libevent_a_CPPFLAGS = $(AM_CPPFLAGS)

librtmon_a_CPPFLAGS = $(AM_CPPFLAGS)

libtimer_a_CPPFLAGS = $(AM_CPPFLAGS)


# include

include_HEADERS = include/rtems.h

# include/rtems/rtems
include_rtems_rtemsdir = $(includedir)/rtems/rtems

include_rtems_rtems_HEADERS = \
    include/rtems/rtems/attr.h \
    include/rtems/rtems/cache.h \
    include/rtems/rtems/clock.h include/rtems/rtems/config.h \
    include/rtems/rtems/event.h \
    include/rtems/rtems/eventset.h include/rtems/rtems/intr.h \
    include/rtems/rtems/message.h include/rtems/rtems/modes.h \
    include/rtems/rtems/options.h \
    include/rtems/rtems/ratemon.h \
    include/rtems/rtems/rtemsapi.h \
    include/rtems/rtems/sem.h \
    include/rtems/rtems/status.h \
    include/rtems/rtems/support.h \
    include/rtems/rtems/tasks.h \
    include/rtems/rtems/timer.h include/rtems/rtems/types.h


include_rtems_rtems_HEADERS +=  \
    inline/rtems/rtems/attr.inl \
    inline/rtems/rtems/eventset.inl \
    inline/rtems/rtems/message.inl inline/rtems/rtems/modes.inl \
    inline/rtems/rtems/options.inl \
    inline/rtems/rtems/ratemon.inl  \
    inline/rtems/rtems/sem.inl inline/rtems/rtems/status.inl \
    inline/rtems/rtems/support.inl inline/rtems/rtems/tasks.inl \
    inline/rtems/rtems/timer.inl

# MANAGERS INITIALIZATIONS
librtems_a_SOURCES =      

## TASK_C_FILES 
librtems_a_SOURCES += src/tasks.c src/taskcreate.c src/taskdelete.c \
    src/taskgetnote.c src/taskident.c src/taskinitusers.c \
    src/taskissuspended.c src/taskmode.c src/taskrestart.c src/taskresume.c \
    src/tasksetnote.c src/tasksetpriority.c src/taskstart.c \
    src/tasksuspend.c src/taskwakeafter.c src/taskwakewhen.c \
    src/taskvariableadd.c src/taskvariabledelete.c src/taskvariableget.c

## RATEMON_C_FILES 
librtmon_a_SOURCES = src/ratemoncancel.c src/ratemoncreate.c \
    src/ratemondelete.c src/ratemongetstatus.c src/ratemonident.c \
    src/ratemonperiod.c src/ratemontimeout.c src/ratemon.c

## INTR_C_FILES 
librtems_a_SOURCES += src/intr.c src/intrcatch.c 

## CLOCK_C_FILES
librtems_a_SOURCES += src/clockget.c src/clockset.c \
    src/clocktick.c src/clocksetnsecshandler.c src/clockgetuptime.c \
    src/clocktodtoseconds.c src/clocktodvalidate.c

## TIMER_C_FILES
libtimer_a_SOURCES = src/rtemstimer.c src/timercancel.c src/timercreate.c \
    src/timerdelete.c src/timerfireafter.c src/timerfirewhen.c \
    src/timerident.c src/timerreset.c src/timerserver.c \
    src/timerserverfireafter.c src/timerserverfirewhen.c 

## MESSAGE_QUEUE_C_FILES
libmsg_a_SOURCES = src/msg.c src/msgqallocate.c src/msgqbroadcast.c \
    src/msgqcreate.c src/msgqdelete.c src/msgqflush.c \
    src/msgqgetnumberpending.c src/msgqident.c src/msgqreceive.c \
    src/msgqsend.c c src/msgqtranslatereturncode.c src/msgqurgent.c

## SEMAPHORE_C_FILES
libsem_a_SOURCES = src/sem.c src/semcreate.c src/semdelete.c src/semident.c \
    src/semobtain.c src/semrelease.c src/semflush.c \
    src/semtranslatereturncode.c 

## EVENT_C_FILES
libevent_a_SOURCES = src/event.c src/eventreceive.c src/eventseize.c \
    src/eventsend.c src/eventsurrender.c src/eventtimeout.c

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
