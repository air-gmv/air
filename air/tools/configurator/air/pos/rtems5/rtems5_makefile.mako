<%
    import os
    import air
    import utils.templates as makoutils

    # base build dir
    base_dir = os.path.join(
        os.path.relpath(air.INSTALL_DIRECTORY, output_dir),
        os.path.relpath(output_dir, air.ROOT_DIRECTORY))

    # rtems build dir
    build_dir = os.path.join(
        os.path.relpath(air.INSTALL_DIRECTORY, output_dir),
        os.path.relpath(output_dir, air.ROOT_DIRECTORY),
        'rtems5-build')

    # rtems install dir
    install_dir = os.path.join(
        air.INSTALL_DIRECTORY,
        os.path.relpath(output_dir, air.ROOT_DIRECTORY),
        'rtems5-install')

    # get libair headers
    libair_headers = os_configuration.get_libair_headers()
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("POS RTEMS-5.0")}\

# Makefile Include file
${template.MakefileInc()}

CURRENT_PATH=$(shell pwd)

# reguired AIR headers
AIR_HEADERS=${'\\'}
-DAIR_HYPERVISOR${'\\'}
-I$(AIR_ROOT)/libs/libprintf/include${'\\'}
${template.get_headers_directories(libair_headers)}\

# RTEMS build and install directories
RTEMS_BUILD_DIR=${build_dir}
RTEMS_INSTALL_DIR=${install_dir}

# All
${template.Rule('all', True, ['rtems5'])}

# RTEMS 5
${template.Rule('rtems5', True, None)}
${'\t'}if [ ! -d "$(RTEMS_BUILD_DIR)" ]; then ${'\\'}
${'\t'}$(MKDIR) $(RTEMS_BUILD_DIR) && ${'\\'}
${'\t'}cd $(RTEMS_BUILD_DIR); $(CURRENT_PATH)/rtems5/configure ${'\\'}
${'\t\t'}--target=$(RTEMS5_TARGET) ${'\\'}
${'\t\t'}--enable-rtems-inlines ${'\\'}
${'\t\t'}--disable-itron ${'\\'}
${'\t\t'}--disable-multiprocessing ${'\\'}
${'\t\t'}--enable-posix ${'\\'}
${'\t\t'}--disable-networking ${'\\'}
${'\t\t'}--disable-cxx ${'\\'}
${'\t\t'}--disable-tests ${'\\'}
${'\t\t'}--enable-smp ${'\\'}
${'\t\t'}--enable-rtemsbsp=$(RTEMS5_BSP) ${'\\'}
${'\t\t'}--prefix=$(RTEMS_INSTALL_DIR) && cd ..; ${'\\'}
${'\t'}fi
${'\t'}make -C $(RTEMS_BUILD_DIR) CPPFLAGS='$(AIR_HEADERS)' && ${'\\'}
${'\t'}make -C $(RTEMS_BUILD_DIR) install

# RTEMS 5 with no configure
${template.Rule('noconf', True, None)}
${template.Remove(install_dir)}
${'\t'}make -C $(RTEMS_BUILD_DIR) CPPFLAGS='$(AIR_HEADERS)' && ${'\\'}
${'\t'}make -C $(RTEMS_BUILD_DIR) install

# Clean
${template.Rule('clean', True, None)}
${template.Remove(base_dir)}

# Distclean
${template.Rule('distclean', True, None)}
${template.Remove(base_dir)}
${template.Remove(output_file)}


