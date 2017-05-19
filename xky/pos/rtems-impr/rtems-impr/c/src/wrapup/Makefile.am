##
##  $Id: Makefile.am 373 2008-10-23 16:46:12Z mcoutinho $
##
##  build and install "glommed" librtemsbsp.a
##

include $(top_srcdir)/automake/compile.am

## Setup the variant build subdirectory
project_lib_DATA = o-optimize/librtemsbsp.a
CLEANFILES = o-optimize/librtemsbsp.a


if HAS_LIBBSP
SRCS = ../lib/libbsp/@RTEMS_CPU@/@RTEMS_BSP_FAMILY@/libbsp.a
endif

# FIXME: ATM, there is no libcpu.a, anymore.
# SRCS += $(wildcard $(PROJECT_LIB)/libcpu.a)


o-optimize/librtemsbsp.a: $(SRCS)
	rm -f $@
	$(MKDIR_P) o-optimize
	rm -rf o-optimize/*.a o-optimize/*.$(OBJEXT) o-optimize/*.rel
	for f in $(SRCS); do \
	  case $$f in \
	  *.$(OBJEXT) | *.rel) \
	    if test -f o-optimize/`basename $$f`; then \
	      if cmp $$f o-optimize/`basename $$f`; then \
		true; \
	      else \
		echo 1>&2 "ERROR -- `basename $$f` in multiple files"; \
		exit 1; \
	      fi; \
	    else \
	      cp $$f o-optimize/; \
	      chmod a-w o-optimize/`basename $$f`; \
	    fi; \
	    ;; \
	  *.a) \
	    cd o-optimize; \
	      $(AR) xv ../$$f || exit 1; \
	      chmod a-w * ; \
            cd ..; \
	    ;; \
	  esac; \
	done
#TODO: remove xpto.txt and place the original values
	if [ "`ls o-optimize/*.$(OBJEXT) 2>/dev/null | wc -l`" -gt "0" ]; then ls o-optimize/*.$(OBJEXT) o-optimize/*.rel > xpto.txt; else ls o-optimize/*.rel > xpto.txt; fi
	$(AR) rc $@ @xpto.txt
#	rm -f xpto.txt o-optimize/*.$(OBJEXT) o-optimize/*.rel
	$(RANLIB) $@

	
include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am

