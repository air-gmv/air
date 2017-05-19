<%
    import os
    import xky
    import utils.templates as makoutils

    build_dir = os.path.relpath(xky.INSTALL_TOOL_DIRECTORY, output_dir)
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("Partition assembler")}

HOST_BUILD=${build_dir}

# Makefile Include file
${template.MakefileInc()}

EXE=${build_dir}/partition_assembler
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,$(HOST_BUILD)/%.o,$(SRCS))
HOST_CPPFLAGS=-Wall -O2 -B./

# All
${template.Rule('all', True, [ '$(EXE)' ])}
all: $(EXE) $(OBJS)

# Executable
${template.Rule('$(EXE)', False, ['$(OBJS)'])}
${'\t'}$(HOST_CC) $(OBJS) -o $(EXE)

# Clean
${template.Rule('clean', False, None)}
${template.Remove(build_dir)}

# Distclean
${template.Rule('distclean', True, None)}
${template.Remove(build_dir)}
${template.Remove(output_file)}

