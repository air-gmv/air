<%
    import os
    import air
    import utils
    import utils.file as fileutils
    import utils.templates as makoutils

    # get Makefile parameters
    lib_name, lib_make = makoutils.getMakefileLib(input_file)
    build_dir = os.path.relpath(air.INSTALL_PMK_DIRECTORY, air.SOURCE_PMK_DIRECTORY)

    # get configuration memory requirements
    mmap = os_configuration.get_memory_map()

    # ASM offsets
    asm_source = os.path.join('arch', os_configuration.arch, 'asm_offsets.c')
    asm_header = os.path.join('arch', os_configuration.arch, 'include', 'asm_offsets.h')
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("PMK - Partition Management Kernel")}

TARGET_BUILD=${build_dir}
${lib_make}=${os.path.join(build_dir, makoutils.getLibName(lib_name))}

# Makefile Include file
${template.MakefileInc()}

<%
    kernel_headers = [os.path.relpath(h, output_dir) for h in os_configuration.get_kernel_headers()]
%>\
PMK_HEADERS=${'\\'}
${template.FileList(kernel_headers)}\

<%
    kernel_sources = [os.path.relpath(s, output_dir) for s in os_configuration.get_kernel_sources()]
%>\
PMK_SOURCES=${'\\'}
${template.FileList(kernel_sources)}\

PMK_OBJECTS=$(patsubst %.c,$(TARGET_BUILD)/%.o,${'\\'}
            $(patsubst %.S,$(TARGET_BUILD)/%.o,$(PMK_SOURCES)))

TARGET_CPPFLAGS+=${'\\'}
-DPMK_MAX_CORES=${os_configuration.get_available_cores()}${'\\'}
-DDEBUG_MONITOR=${os_configuration.debug_monitor}${'\\'}
-DPMK_${os_configuration.arch.upper()}_${os_configuration.bsp.upper()}${'\\'}
-DPMK_PARTITION_BASE_ADDR=${hex(mmap.partition_space[0])}${'\\'}
<%
    defines      = os_configuration.get_target_defines()
%>\
% for i, define in enumerate(defines):
-D${define}${'\\' if i < len(define) or len(include_dirs) > 0 else ''}
% endfor
${template.get_headers_directories(kernel_headers)}\

# All
${template.Rule('all', True, [ '$({0})'.format(lib_make) ])}

# Assembly Offsets
${template.Rule(asm_header, True, None)}
${'\t'}$(TARGET_CC) $(TARGET_CPPFLAGS) ${asm_source} -S -o asm_offsets.s
${'\t'}${air.SOURCE_TOOLS_DIRECTORY}/configurator/utils/asm_offsets.py asm_offsets.s  $@
${template.Remove('asm_offsets.s')}

# PMK
${template.Rule('$({0})'.format(lib_make), False, [ asm_header, '$(PMK_OBJECTS)', '$(PMK_HEADERS)' ])}
${template.MakeLibrary('$@', '$(PMK_OBJECTS)')}
${'\t'}@$(foreach obj,$(PMK_HEADERS),$(MKDIR) $(dir ${build_dir}/$(obj));)
${'\t'}@$(foreach obj,$(PMK_HEADERS),$(CP) $(obj) ${build_dir}/$(obj);)

# LIBAIR

# Clean
${template.Rule('clean', True, None)}
${template.Remove(asm_header)}
${template.Remove('$(TARGET_BUILD)')}

# Distclean
${template.Rule('distclean', True, None)}
${template.Remove(asm_header)}
${template.Remove('$(TARGET_BUILD)')}
${template.Remove(output_file)}

