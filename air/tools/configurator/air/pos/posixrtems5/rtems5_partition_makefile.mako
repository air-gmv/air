<%
    import os
    import air
    import utils.templates as makoutils

    # get Makefile parameters
    lib_name, lib_make = makoutils.getMakefileLib(input_file)
    build_dir = os.path.relpath(air.INSTALL_PMK_DIRECTORY, air.SOURCE_PMK_DIRECTORY)

%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader('{0} : {1}'.format(lib_name.upper(), partition))}\

EXEC=${partition.executable}
PGM=$(EXEC)
 
# Optional managers required by the application, select between:
#     Event Manager = event
#     Message Queue Manager = msg
#     Rate Monotonic Manager = rtmon
#     Semaphore Manager = sem
#     Timer manager = timer
#     Extension Manager = ext
#     IO Manager = io
MANAGERS=sem rtmon msg timer io

# C source code and headers filenames used in the example
CSRCS=$(shell find ./ -type f -name '*.c')
CHDRS=$(shell find ./ -type f -name '*.h')
COBJS_=$(CSRCS:.c=.o)
COBJS=$(COBJS_:%=$(ARCH)/%) $(shell find ../common -type f -name '*.o')
 
# RTEMS Newlib check (TBC)
RTEMS_USES_NEWLIB = no
 
# Example of assembly source code filenames that could be included 
ASSRCS=$(shell find ./ -type f -name '*.S')
ASOBJS_=$(ASSRCS:.S=.o)
ASOBJS=$(ASOBJS_:%=$(ARCH)/%)

# libraries
LD_LIBS =${'\\'}
% for i, libname in enumerate(partition.libraries):
$(AIR_LIBS)/${libname.lower()}/${libname.lower()}.a${'\\' if i < len(partition.libraries) - 1 else '\\'}
% endfor

# The RTEMS_MAKEFILE_PATH is defined by the user for the specific CPU and BSP
RTEMS_MAKEFILE_PATH=$(AIR_POS)/${os.path.join('rtems5', 'rtems5-install', 'sparc-rtems5', 'leon3')}

# These includes should not be modified by the user.  
include $(RTEMS_MAKEFILE_PATH)/Makefile.inc 
include $(RTEMS_CUSTOM) 
include $(PROJECT_ROOT)/make/leaf.cfg

# built in libraries include files
CPPFLAGS+=${'\\'}
-DAIR_HYPERVISOR./${'\\'}
-I./${'\\'}
-B./${'\\'}
-I../common/${'\\'}
-B../common/${'\\'}
${template.LibraryIncludes(partition.libraries)}\

# build LD falgs
LDFLAGS += -Wl,--gc-sections -Wl,--wrap=printf -Wl,--wrap=puts -Wl,--wrap=putchar

OBJS = $(COBJS) $(ASOBJS)
 
all:    $(ARCH) $(PGM)  
    
$(PGM): $(OBJS) $(CHDRS)
${'\t'}$(make-exe)
