##
## $Id: Makefile.am 9617 2011-02-23 19:07:21Z mcoutinho $
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

## Setup the variant build subdirectory
ARCH = o-optimize

project_lib_LIBRARIES = librtemscpu.a libsem.a libmsg.a libevent.a librtmon.a libtimer.a libext.a libio.a
librtemscpu_a_SOURCES =

TMP_LIBS =
TMP_LIBS += ../score/cpu/@RTEMS_CPU@/libscorecpu.a
TMP_LIBS += ../score/libscore.a
TMP_LIBS += ../sapi/libsapi.a
TMP_LIBS += ../rtems/librtems.a

TMP_LIBS_SEM = ../rtems/libsem.a
TMP_LIBS_MSG = ../rtems/libmsg.a
TMP_LIBS_EVENT = ../rtems/libevent.a
TMP_LIBS_RATEMON = ../rtems/librtmon.a
TMP_LIBS_TIMER = ../rtems/libtimer.a
TMP_LIBS_EXT = ../sapi/libext.a
TMP_LIBS_IO = ../sapi/libio.a

librtemscpu.a: $(TMP_LIBS)
	rm -f $@
	$(MKDIR_P) $(ARCH)
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(TMP_LIBS); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	ls $(ARCH)/* > $@-list
	$(AR) rc $@ @$@-list
#	rm -f $@-list $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@

libsem.a: $(TMP_LIBS_SEM)
	rm -f $@
	$(MKDIR_P) $(ARCH)
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(TMP_LIBS_SEM); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	ls $(ARCH)/* > $@-list
	$(AR) rc $@ @$@-list
#	rm -f $@-list $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@

libmsg.a: $(TMP_LIBS_MSG)
	rm -f $@
	$(MKDIR_P) $(ARCH)
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(TMP_LIBS_MSG); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	ls $(ARCH)/* > $@-list
	$(AR) rc $@ @$@-list
#	rm -f $@-list $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@

libevent.a: $(TMP_LIBS_EVENT)
	rm -f $@
	$(MKDIR_P) $(ARCH)
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(TMP_LIBS_EVENT); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	ls $(ARCH)/* > $@-list
	$(AR) rc $@ @$@-list
#	rm -f $@-list $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@


librtmon.a: $(TMP_LIBS_RATEMON)
	rm -f $@
	$(MKDIR_P) $(ARCH)
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(TMP_LIBS_RATEMON); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	ls $(ARCH)/* > $@-list
	$(AR) rc $@ @$@-list
#	rm -f $@-list $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@

libtimer.a: $(TMP_LIBS_TIMER)
	rm -f $@
	$(MKDIR_P) $(ARCH)
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(TMP_LIBS_TIMER); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	ls $(ARCH)/* > $@-list
	$(AR) rc $@ @$@-list
#	rm -f $@-list $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@

libext.a: $(TMP_LIBS_EXT)
	rm -f $@
	$(MKDIR_P) $(ARCH)
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(TMP_LIBS_EXT); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	ls $(ARCH)/* > $@-list
	$(AR) rc $@ @$@-list
#	rm -f $@-list $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@

libio.a: $(TMP_LIBS_IO)
	rm -f $@
	$(MKDIR_P) $(ARCH)
	rm -rf $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	for f in $(TMP_LIBS_IO); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f $(ARCH)/`basename $$f`; then \
	      if cmp $$f $(ARCH)/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f $(ARCH)/; \
	      chmod a-w $(ARCH)/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd $(ARCH); \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
	ls $(ARCH)/* > $@-list
	$(AR) rc $@ @$@-list
#	rm -f $@-list $(ARCH)/*.$(OBJEXT) $(ARCH)/*.rel
	$(RANLIB) $@

all-local: $(TMPINSTALL_FILES)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
