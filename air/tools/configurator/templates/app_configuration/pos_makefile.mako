<%
    import os
    import air
    import utils
    import utils.file as fileutils
    import utils.templates as makoutils

    build_dir = 'build'

    if partition.is_system:
        kernel_headers_dirs = set([os.path.dirname(d) for d in os_configuration.get_kernel_headers()])
        kernel_headers = [
            os.path.join('$(AIR_PMK)', os.path.relpath(h, air.SOURCE_PMK_DIRECTORY))
            for h in kernel_headers_dirs]
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader('{0} : {1}'.format(pos_config.name, partition))}

EXEC=${partition.executable}
TARGET_BUILD=${build_dir}

# AIR Makefile Include file
${template.MakefileInc(True)}\

CSRCS=$(shell find ./ ../common -type f -name '*.c')
CHDRS=$(shell find ./ ../common -type f -name '*.h')
ASSRCS=$(shell find ./ ../common -type f -name '*.S')

SOURCE_FILES=$(CSRCS) $(ASSRCS)
OBJECT_FILES=${'\\'}
$(patsubst %.c,$(TARGET_BUILD)/%.o,${'\\'}
$(patsubst %.S,$(TARGET_BUILD)/%.o,$(SOURCE_FILES)))

# built in libraries include files
TARGET_CPPFLAGS+=${'\\'}
% if partition.is_system:
-DPMK_MAX_CORES=${os_configuration.get_available_cores()}${'\\'}
%if os_configuration.arch == "arm":
-DCPU_LITTLE_ENDIAN${'\\'}
%endif
% for i, directory in enumerate(kernel_headers):
-I${directory}${'\\'}
% endfor
% endif
-I./${'\\'}
-B./${'\\'}
-I../common/${'\\'}
-B../common/${'\\'}
-B$(AIR_POS)/${pos_config.name}/${'\\'}
${template.LibraryIncludes(partition.libraries)}\


# All
${template.Rule('all', True, ['$(EXEC)', '$(OBJECT_FILES)', '$(CHDRS)'])}

# Executale
${template.Rule('$(EXEC)', False, ['$(OBJECT_FILES)', '$(CHDRS)'])}
${'\t'}$(TARGET_LD)${'\\'}
${'\t\t'}$(TARGET_LDFLAGS)${'\\'}
${'\t\t'}-nostdlib -nodefaultlibs -nostartfiles${'\\'}
${'\t\t'}-T${os.path.join('$(AIR_POS)', pos_config.name, 'include', 'linkcmds.ld')}${'\\'}
${'\t\t'}--start-group${'\\'}
${'\t\t'}--build-id=none${'\\'}
% if partition.is_system:
${'\t\t'}$(AIR_PMK)/pmk.a${'\\'}
% endif
${'\t\t'}${os.path.join('$(AIR_POS)', pos_config.name, '{0}.a'.format(pos_config.name))}${'\\'}
% for i, libname in enumerate(partition.libraries):
${'\t\t'}${os.path.join('$(AIR_LIBS)', libname.lower(), '{0}.a'.format(libname.lower()))}${'\\'}
% endfor
${'\t\t'}$(OBJECT_FILES)${'\\'}
${'\t\t'}--end-group${'\\'}
${'\t\t'}-o $(EXEC)

# Clean
${template.Rule('clean', True, None)}
${template.Remove('$(TARGET_BUILD)')}
${template.Remove('$(EXEC)')}



