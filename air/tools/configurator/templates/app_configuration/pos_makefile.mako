<%
    import os
    import air
    import utils
    import utils.file as fileutils
    import utils.templates as makoutils

    build_dir = 'build'
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
${'\t\t'}-nostdlib -nodefaultlibs -nostartfiles${'\\'}
${'\t\t'}-T${os.path.join('$(AIR_POS)', pos_config.name, 'include', 'linkcmds.ld')}${'\\'}
${'\t\t'}-Wl,--start-group${'\\'}
${'\t\t'}-Wl,--build-id=none${'\\'}
${'\t\t'}-L$(AIR_PMK)/${'\\'}
${'\t\t'}${os.path.join('$(AIR_POS)', pos_config.name, '{0}.a'.format(pos_config.name))}${'\\'}
${'\t\t'}-lgcc -lc -lm${'\\'}
% for i, libname in enumerate(partition.libraries):
${'\t\t'}${os.path.join('$(AIR_LIBS)', libname.lower(), '{0}.a'.format(libname.lower()))}${'\\'}
% endfor
${'\t\t'}$(OBJECT_FILES)${'\\'}
${'\t\t'}-Wl,--end-group${'\\'}
${'\t\t'}-o $(EXEC)

# Clean
${template.Rule('clean', True, None)}
${template.Remove('$(TARGET_BUILD)')}
${template.Remove('$(EXEC)')}



