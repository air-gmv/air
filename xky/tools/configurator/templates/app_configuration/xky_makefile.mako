<%
    import os
    import xky
    import utils
    import utils.file as fileutils
    import utils.templates as makoutils

    # get Makefile parameters
    lib_name, lib_make = makoutils.getMakefileLib(input_file)
    build_dir = os.path.join('..', 'executable')

    # get kernel headers
    kernel_headers = [
        os.path.join('$(XKY_PMK)', os.path.relpath(h, xky.SOURCE_PMK_DIRECTORY))
        for h in os_configuration.get_kernel_headers()]

    mmap = os_configuration.get_memory_map()
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("XKY Application Main Makefile")}\

TARGET_BUILD=${'build'}
XKY_APP=${os.path.join( os.path.join('..', 'executable'), 'XKYAPP.exe')}

# Makefile Include file
${template.MakefileInc(True)}

SOURCE_FILES=${'\\'}
usr_config.c${'\\'}
usr_arch.c${'\\'}
usr_hm.c${'\\'}
usr_channels.c${'\\'}
usr_schedules.c${'\\'}
usr_partitions.c${'\\'}
usr_sharedmemory.c${'\\'}
usr_partitions_data.c

OBJECT_FILES=$(patsubst %.c,$(TARGET_BUILD)/%.o,${'\\'}
             $(patsubst %.S,$(TARGET_BUILD)/%.o,$(SOURCE_FILES)))

TARGET_CPPFLAGS+=${'\\'}
-DPMK_MAX_CORES=${os_configuration.get_available_cores()}${'\\'}
-DPMK_${os_configuration.arch.upper()}_${os_configuration.bsp.upper()}${'\\'}
-fno-zero-initialized-in-bss${'\\'}
-I./${'\\'}
${template.get_headers_directories(kernel_headers)}\

# All
${template.Rule('all', True, ['$(XKY_APP)'])}

# Create XKY application
${template.Rule('$(XKY_APP)', False, ['$(OBJECT_FILES)'])}
${'\t'}@(mkdir -p $(dir $(XKY_APP)))
${'\t'}@($(CP) `$(TARGET_CC) -print-file-name=libgcc.a $(TARGET_CPPFLAGS)`  $(TARGET_BUILD)/libgcc.a)
${'\t'}@($(CP) `$(TARGET_CC) -print-file-name=libc.a $(TARGET_CPPFLAGS)`  $(TARGET_BUILD)/libc.a)
${'\t'}@($(CP) $(XKY_PMK)/pmk.a $(TARGET_BUILD)/pmk.a)
${'\t'}# Link Everything
${'\t'}$(TARGET_LD)${'\\'}
${'\t\t'}-nostdlib -nodefaultlibs -nostartfiles${'\\'}
${'\t\t'}-Tlinkcmds.ld${'\\'}
${'\t\t'}-Wl,--build-id=none${'\\'}
${'\t\t'}-Wl,--start-group${'\\'}
${'\t\t'}-L$(XKY_PMK)/${'\\'}
${'\t\t'}$(TARGET_BUILD)/pmk.a${'\\'}
${'\t\t'}$(TARGET_BUILD)/libc.a${'\\'}
${'\t\t'}$(TARGET_BUILD)/libgcc.a${'\\'}
${'\t\t'}$(TARGET_BUILD)/usr_config.o${'\\'}
${'\t\t'}$(TARGET_BUILD)/usr_arch.o${'\\'}
${'\t\t'}$(TARGET_BUILD)/usr_hm.o${'\\'}
${'\t\t'}$(TARGET_BUILD)/usr_channels.o${'\\'}
${'\t\t'}$(TARGET_BUILD)/usr_sharedmemory.o${'\\'}
${'\t\t'}$(TARGET_BUILD)/usr_schedules.o${'\\'}
${'\t\t'}$(TARGET_BUILD)/usr_partitions.o${'\\'}
${'\t\t'}$(TARGET_BUILD)/usr_partitions_data.o${'\\'}
${'\t\t'}-Wl,--end-group${'\\'}
${'\t\t'}-o $(XKY_APP)

# Assemble the partition ELFs into the configurations
${template.Rule('usr_partitions_data.c', True, None)}
${'\t'}@$(XKY_TOOLS)/partition_assembler${'\\'}
% for i, partition in enumerate(app_configuration.partitions):
${'\t\t{0} {1} {2}{3}'.format(
    os.path.join('..', partition.directory, partition.executable),
    '0x{0:08x}'.format(partition.memory_vaddr), '0x{0:08x}'.format(partition.memory_size),
    '\\' if i < len(app_configuration.partitions) - 1 else '')}
% endfor

# Clean
${template.Rule('clean', True, None)}
${template.Remove('$(TARGET_BUILD)')}
${template.Remove('$(dir $(XKY_APP))')}

# Distclean
${template.Rule('distclean', True, ['clean'])}
${template.Remove('usr_partitions_data.c')}

