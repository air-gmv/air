<%
    import air
    import utils
    import utils.file as fileutils
    import utils.templates as makoutils

    from os import path

    # get Makefile parameters
    build_dir = path.relpath(path.join(air.INSTALL_LIBS_DIRECTORY, lib_name), path.join(output_dir))

    library = path.join(build_dir, '{0}.a'.format(lib_name))
    target_sources = [path.relpath(s, output_dir) for s in os_configuration.get_library_sources(lib_name)]
    target_headers = [path.relpath(h, output_dir) for h in os_configuration.get_library_private_headers(lib_name)]
    public_headers = [path.relpath(h, output_dir) for h in os_configuration.get_library_public_headers(lib_name)]
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("{0} - {1}".format(lib_name, lib_description))}

TARGET_BUILD=${build_dir}

TARGET_LIBRARY=${library}

${template.MakefileInc()}

TARGET_HEADERS=${'\\'}
${template.FileList(target_headers)}\

TARGET_SOURCES=${'\\'}
${template.FileList(target_sources)}\

TARGET_OBJECTS=${'\\'}
$(patsubst %.c,$(TARGET_BUILD)/%.o,${'\\'}
$(patsubst %.S,$(TARGET_BUILD)/%.o,$(TARGET_SOURCES)))

TARGET_CPPFLAGS+=${'\\'}
-DPMK_MAX_CORES=${os_configuration.get_available_cores()}${'\\'}
%if os_configuration.arch == "arm":
-DCPU_LITTLE_ENDIAN${'\\'}
%endif
%if os_configuration.bsp == "ultrascale96v2_a53":
-DPMK_ARM_ULTRASCALE96V2_A53${'\\'}
%endif
${template.get_headers_directories(target_headers)}\

LIBRARY_HEADERS=${'\\'}
${template.FileList(public_headers)}\

# All
${template.Rule('all', True, ['$(TARGET_LIBRARY)'])}

# Library
${template.Rule('$(TARGET_LIBRARY)', False, ['$(TARGET_OBJECTS)', '$(TARGET_HEADERS)'])}
${template.MakeLibrary('$@', '$(TARGET_OBJECTS)')}
${template.CopyTo('$(LIBRARY_HEADERS)', path.join(build_dir, 'include'))}

# Clean
${template.Rule('clean', True, None)}
${template.Remove('$(TARGET_BUILD)')}

# Distclean
${template.Rule('distclean', True, ['clean'])}
${template.Remove(output_file)}
