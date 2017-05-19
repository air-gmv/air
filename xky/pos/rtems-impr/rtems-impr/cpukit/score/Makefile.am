##
## $Id: Makefile.am 9617 2011-02-23 19:07:21Z mcoutinho $
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

SUBDIRS = cpu

## include

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/rtems/system.h include/rtems/clockdrv.h

include_rtems_scoredir = $(includedir)/rtems/score

include_rtems_score_HEADERS = include/rtems/score/address.h \
    include/rtems/score/apiext.h \
    include/rtems/score/bitfield.h include/rtems/score/chain.h \
    include/rtems/score/context.h \
    include/rtems/score/coremsg.h \
    include/rtems/score/coremutex.h  \
    include/rtems/score/coresem.h \
    include/rtems/score/heap.h \
    include/rtems/score/interr.h include/rtems/score/isr.h \
    include/rtems/score/object.h include/rtems/score/priority.h \
    include/rtems/score/stack.h include/rtems/score/states.h \
    include/rtems/score/sysstate.h include/rtems/score/thread.h \
    include/rtems/score/threadq.h include/rtems/score/timespec.h \
    include/rtems/score/tod.h include/rtems/score/tqdata.h \
    include/rtems/score/userext.h include/rtems/score/watchdog.h \
    include/rtems/score/wkspace.h


## inline
include_rtems_score_HEADERS += inline/rtems/score/address.inl \
    inline/rtems/score/chain.inl \
    inline/rtems/score/coremsg.inl \
    inline/rtems/score/coremutex.inl inline/rtems/score/coresem.inl \
    inline/rtems/score/heap.inl inline/rtems/score/isr.inl \
    inline/rtems/score/object.inl inline/rtems/score/priority.inl \
    inline/rtems/score/stack.inl inline/rtems/score/states.inl \
    inline/rtems/score/sysstate.inl inline/rtems/score/thread.inl \
    inline/rtems/score/threadq.inl inline/rtems/score/tod.inl \
    inline/rtems/score/tqdata.inl \
    inline/rtems/score/watchdog.inl inline/rtems/score/wkspace.inl

GCOV_FLAGS =
if RTEMS_TEST_HAS_COVERAGE
GCOV_FLAGS += -O0 -fprofile-arcs -ftest-coverage
else
GCOV_FLAGS += -O2
endif

## src
AM_CPPFLAGS += -D__RTEMS_INSIDE__ $(GCOV_FLAGS)

noinst_LIBRARIES = libscore.a
libscore_a_SOURCES =
libscore_a_CPPFLAGS = $(AM_CPPFLAGS)


## CORE_MESSAGE_QUEUE_C_FILES
libscore_a_SOURCES += src/coremsg.c src/coremsgbroadcast.c \
    src/coremsgclose.c src/coremsgflush.c \
    src/coremsginsert.c src/coremsgflushsupp.c src/coremsgseize.c \
    src/coremsgsubmit.c

## CORE_MUTEX_C_FILES
libscore_a_SOURCES += src/coremutex.c src/coremutexflush.c \
    src/coremutexseize.c src/coremutexsurrender.c

## CORE_SEMAPHORE_C_FILES
libscore_a_SOURCES += src/coresem.c src/coresemflush.c \
    src/coresemsurrender.c

## HEAP_C_FILES
libscore_a_SOURCES += src/heap.c src/heapallocate.c \
    src/heapfree.c 
    

## OBJECT_C_FILES
libscore_a_SOURCES += src/object.c src/objectallocate.c \
    src/objectextendinformation.c src/objectfree.c src/objectget.c \
    src/objectgetisr.c   \
    src/objectinitializeinformation.c src/objectnametoid.c \
    src/objectcomparenameraw.c src/objectcomparenamestring.c
    
## THREAD_C_FILES
libscore_a_SOURCES += src/thread.c src/threadchangepriority.c \
    src/threadclearstate.c src/threadclose.c src/threadcreateidle.c \
    src/threaddelayended.c src/threaddispatch.c src/threadevaluatemode.c \
    src/threadhandler.c src/threadidlebody.c \
    src/threadinitialize.c src/threadloadenv.c src/threadready.c \
    src/threadresettimeslice.c src/threadreset.c src/threadrestart.c \
    src/threadresume.c src/threadsetpriority.c \
    src/threadsetstate.c src/threadsettransient.c src/threadstackallocate.c \
    src/threadstackfree.c src/threadstart.c src/threadstartmultitasking.c \
    src/threadsuspend.c src/threadtickletimeslice.c \
    src/threadyieldprocessor.c src/threadfreemutexes.c

## THREADQ_C_FILES
libscore_a_SOURCES += src/threadq.c src/threadqdequeue.c \
    src/threadqdequeuefifo.c src/threadqdequeuepriority.c \
    src/threadqenqueue.c src/threadqenqueuefifo.c \
    src/threadqenqueuepriority.c src/threadqextract.c \
    src/threadqextractfifo.c src/threadqextractpriority.c \
    src/threadqextractwithproxy.c \
    src/threadqflush.c src/threadqrequeue.c \
    src/threadqtimeout.c

## TIMESPEC_C_FILES
libscore_a_SOURCES += src/timespecaddto.c 

## TOD_C_FILES
libscore_a_SOURCES += src/coretod.c src/coretodset.c src/coretodget.c \
    src/coretodgetuptime.c src/coretodtickle.c src/coretodisleapyear.c

## WATCHDOG_C_FILES
libscore_a_SOURCES += src/watchdog.c src/watchdogadjust.c \
    src/watchdoginsert.c src/watchdogremove.c src/watchdogtickle.c

## USEREXT_C_FILES
libscore_a_SOURCES += src/userextaddapiset.c src/userextaddset.c \
    src/userext.c src/userextremoveset.c src/userextthreadbegin.c \
    src/userextthreadcreate.c src/userextthreaddelete.c \
    src/userextthreadrestart.c src/userextthreadstart.c \
    src/userextthreadswitch.c

## STD_C_FILES
libscore_a_SOURCES += src/apiext.c src/chain.c src/chainappend.c \
    src/chainextract.c src/chainget.c \
    src/interr.c src/isr.c src/wkspace.c

## SYSTEMSTATE_C_FILES
libscore_a_SOURCES += src/sysstate.c

EXTRA_DIST = src/Unlimited.txt

DISTCLEANFILES = 

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
