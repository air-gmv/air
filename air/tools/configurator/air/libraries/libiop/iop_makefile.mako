<%
    import os
    import air
    import utils
    import utils.file as fileutils
    import utils.templates as makoutils

    build_dir = os.path.relpath(os.path.join(air.INSTALL_LIBS_DIRECTORY, lib_name), os.path.join(output_dir))
    library = os.path.join(build_dir, '{0}.a'.format(lib_name))

    # get IOP configuration
    iop = os_configuration.get_iop_configuration()

    # source files
    source_files = map(lambda x: os.path.relpath(x, output_dir), utils.flatten(
        [
            fileutils.getFilesByExtensions(output_dir, [ '.c' ], False),
            [ fileutils.getFilesByExtensions(
                os.path.join(output_dir, 'drivers', p), [ '.c' ], False) for p in iop.drivers ]
        ]))
    
    # header files
    header_files = map(lambda x: os.path.relpath(x, output_dir), utils.flatten([
            # common includes
            fileutils.getFilesByExtensions(os.path.join(output_dir, 'include'), [ '.h' ], True),
            # driver includes
            [ fileutils.getFilesByExtensions(
                os.path.join(output_dir, 'drivers', p), [ '.h' ], True) for p in iop.drivers ],
            # dependencies includes
            os_configuration.get_library_private_headers('libiop')
        ]))

    # header files
    library_headers = map(lambda x: os.path.relpath(x, output_dir), utils.flatten([
            # common includes
            fileutils.getFilesByExtensions(os.path.join(output_dir, 'include'), [ '.h' ], True),
            # driver includes
            [ fileutils.getFilesByExtensions(
                os.path.join(output_dir, 'drivers', p), [ '.h' ], True) for p in iop.drivers ]
        ]))
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("LIB iop - I/O Partition library")}\

TARGET_BUILD=${build_dir}

TARGET_LIBRARY=${library}

# Makefile Include file
${template.MakefileInc()}

TARGET_HEADERS=${'\\'}
${template.FileList(header_files)}\

LIBRARY_HEADERS=${'\\'}
${template.FileList(library_headers)}\

RTEMS_MAKEFILE_PATH=${'\\'}
${os.path.relpath(os.path.join(air.INSTALL_POS_DIRECTORY, '$(RTEMS5_DIRECTORY)', '$(RTEMS5_DIRECTORY)-install', '$(RTEMS5_TARGET)', '$(RTEMS5_BSP)'), output_dir)}

ifneq ($(wildcard $(RTEMS_MAKEFILE_PATH)/Makefile.inc),)

RTEMS_USES_NEWLIB = no

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

TARGET_CPPFLAGS+=${'\\'}
-B$(RTEMS_MAKEFILE_PATH)/lib${'\\'}
-DAIR_HYPERVISOR${'\\'}
${template.get_headers_directories(header_files)}${'\\'}

SOURCE_FILES=${'\\'}
${template.FileList(source_files)}

OBJECT_FILES=${'\\'}
$(patsubst %.c,$(TARGET_BUILD)/%.o,${'\\'}
$(patsubst %.S,$(TARGET_BUILD)/%.o,$(SOURCE_FILES)))

# All
${template.Rule('all', True, ['$(TARGET_LIBRARY)'])}

# Library
${template.Rule('$(TARGET_LIBRARY)', False, ['$(OBJECT_FILES)', '$(TARGET_HEADERS)'])}
${template.MakeLibrary('$@', '$(OBJECT_FILES)')}
${template.CopyTo('$(LIBRARY_HEADERS)', os.path.join(build_dir, 'include'))}

else

# All
${template.Rule('all', True, [])}
${'\t'}$(warning RTEMS is missing or not compiled)

endif

# Clean
${template.Rule('clean', True, None)}
${template.Remove('$(TARGET_BUILD)')}

# Distclean
${template.Rule('distclean', True, ['clean'])}
${template.Remove(output_file)}
