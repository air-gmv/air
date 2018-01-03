<%
    import os
    import xky
    import utils.templates as makoutils

    # base build dir
    base_dir = os.path.join(
        os.path.relpath(xky.INSTALL_DIRECTORY, output_dir),
        os.path.relpath(output_dir, xky.ROOT_DIRECTORY))
    
    # rtems build dir
    build_dir = os.path.join(
        os.path.relpath(xky.INSTALL_DIRECTORY, output_dir),
        os.path.relpath(output_dir, xky.ROOT_DIRECTORY),
        'rtems-impr-build')
    
    # rtems install dir
    install_dir = os.path.join(
        xky.INSTALL_DIRECTORY,
        os.path.relpath(output_dir, xky.ROOT_DIRECTORY),
        'rtems-impr-install')

    # get libxky headers
    libxky_headers = os_configuration.get_libxky_headers()
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("POS RTEMS-IMPR - RTEMS-IMPR personality")}\

# Makefile Include file
${template.MakefileInc()}

CURRENT_PATH=$(shell pwd)

# reguired XKY headers
XKY_HEADERS=${'\\'}
${template.get_headers_directories(libxky_headers)}\

# RTEMS build and install directories
RTEMS_BUILD_DIR=${build_dir}
RTEMS_INSTALL_DIR=${install_dir}

# All
${template.Rule('all', True, ['rtems-impr'])}

# RTEMS 4.12
${template.Rule('rtems-impr', True, None)}
${'\t'}if [ ! -d "$(RTEMS_BUILD_DIR)" ]; then ${'\\'}
${'\t'}$(MKDIR) $(RTEMS_BUILD_DIR) && ${'\\'}
${'\t'}cd $(RTEMS_BUILD_DIR); $(CURRENT_PATH)/rtems-impr/configure ${'\\'}
${'\t\t'}--target=$(RTEMS_TARGET) ${'\\'}
${'\t\t'}--enable-rtems-inlines ${'\\'}
${'\t\t'}--disable-itron ${'\\'}
${'\t\t'}--disable-multiprocessing ${'\\'}
${'\t\t'}--disable-posix ${'\\'}
${'\t\t'}--disable-networking ${'\\'}
${'\t\t'}--disable-cxx ${'\\'}
${'\t\t'}--disable-tests ${'\\'}
${'\t\t'}--enable-rtemsbsp=$(RTEMS_BSP) ${'\\'}
${'\t\t'}--prefix=$(RTEMS_INSTALL_DIR) && cd ..; ${'\\'}
${'\t'}fi
${'\t'}make -C $(RTEMS_BUILD_DIR) CPPFLAGS='$(XKY_HEADERS)' && ${'\\'}
${'\t'}make -C $(RTEMS_BUILD_DIR) install


# Clean
${template.Rule('clean', True, None)}
${template.Remove(base_dir)}

# Distclean
${template.Rule('distclean', True, None)}
${template.Remove(base_dir)}
${template.Remove(output_file)}


