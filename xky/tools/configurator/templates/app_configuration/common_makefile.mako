<%
    import os
    import xky
    import utils
    import utils.file as fileutils
    import utils.templates as makoutils

    lib_name, lib_make = makoutils.getMakefileLib(input_file)
    build_dir = 'build'

    lib_name = input_file.replace("_makefile.mako", "")
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader('{0} : {1}'.format('Common', 'Shared Source files'))}\

TARGET_BUILD=${build_dir}

# XKY Makefile Include file
${template.MakefileInc(True)}

CSRCS=$(shell find ./ -type f -name '*.c')
CHDRS=$(shell find ./ -type f -name '*.h')
ASSRCS=$(shell find ./ -type f -name '*.S')

SOURCE_FILES=$(CSRCS) $(ASSRCS)
OBJECT_FILES=$(patsubst %.c,$(TARGET_BUILD)/%.o,${'\\'}
             $(patsubst %.S,$(TARGET_BUILD)/%.o,$(SOURCE_FILES)))

# built in libraries include files
TARGET_CPPFLAGS+=${'\\'}
-I./${'\\'}
-B./${'\\'}

# All
${template.Rule('all', True, [ '$(OBJECT_FILES)', '$(CHDRS)'])}

# Clean
${template.Rule('clean', True, None)}
${template.Remove('$(TARGET_BUILD)')}

# Distclean
${template.Rule('distclean', True, ['clean'])}
${template.Remove(output_file)}

