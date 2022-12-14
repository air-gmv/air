<%
    import os
    import air
    import utils.templates as makoutils

    # get Makefile parameters
    lib_name, lib_make = makoutils.getMakefileLib(input_file)
    build_dir = os.path.relpath(air.INSTALL_PMK_DIRECTORY, air.SOURCE_PMK_DIRECTORY)

    if partition.is_system:
        kernel_headers_dirs = set([os.path.dirname(d) for d in os_configuration.get_kernel_headers()])
        kernel_headers = [
            os.path.join('$(AIR_PMK)', os.path.relpath(h, air.SOURCE_PMK_DIRECTORY))
            for h in kernel_headers_dirs]
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
MANAGERS=sem rtmon msg timer

# C source code and headers filenames used in the example
CSRCS=$(shell find ./ -type f -name '*.c')
CHDRS=$(shell find ./ -type f -name '*.h')
COBJS_=$(CSRCS:.c=.o)
COBJS=$(COBJS_:%=$(ARCH)/%) $(shell find ../common -type f -name '*.o')
 
# RTEMS Improvement RTEMS source code 
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
% if partition.is_system:
$(AIR_PMK)/pmk.a
% endif

# The RTEMS_MAKEFILE_PATH is defined by the user for the specific CPU and BSP
RTEMS_MAKEFILE_PATH=$(AIR_POS)/${os.path.join(pos_config.name, 'rtems48i-install', 'sparc-rtems5', 'leon3')}
RTEMS_EXTERNAL_LIBRARY_PATH=${'\\'}
$(AIR_POS)/${os.path.join(pos_config.name, 'edilib', 'library')}${'\\' if len(partition.libraries) > 0 else ''}

# These includes should not be modified by the user.  
include $(RTEMS_MAKEFILE_PATH)/Makefile.inc 
include $(RTEMS_CUSTOM) 
include $(PROJECT_ROOT)/make/leaf.cfg

# built in libraries include files
CPPFLAGS+=${'\\'}
-DPMK_${os_configuration.arch.upper()}_${os_configuration.bsp.upper()}${'\\'}
% if partition.is_system:
-DPMK_MAX_CORES=${os_configuration.get_available_cores()}${'\\'}
% for i, directory in enumerate(kernel_headers):
-I${directory}${'\\'}
% endfor
% endif
-I./${'\\'}
-B./${'\\'}
-I../common/${'\\'}
-B../common/${'\\'}
-DRTEMS48I${'\\'}
${template.LibraryIncludes(partition.libraries)}\


OBJS = $(COBJS) $(ASOBJS)
 
all:    $(ARCH) $(PGM)  
    
$(PGM): $(OBJS) $(CHDRS)
${'\t'}$(make-exe)
