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
        'rtems48i-build')
    
    # rtems install dir
    install_dir = os.path.join(
        air.INSTALL_DIRECTORY,
        os.path.relpath(output_dir, air.ROOT_DIRECTORY),
        'rtems48i-install')

    # get libair headers
    libair_headers = os_configuration.get_libair_headers()
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("POS RTEMS-IMPR - RTEMS-IMPR personality")}\

# Makefile Include file
${template.MakefileInc2()}

CURRENT_PATH=$(shell pwd)

# reguired AIR headers
AIR_HEADERS=${'\\'}
${template.get_headers_directories(libair_headers)}\

# RTEMS build and install directories
RTEMS_BUILD_DIR=${build_dir}
RTEMS_INSTALL_DIR=${install_dir}
EDILIB_BUILD_DIR=../${base_dir}/edilib

# All
${template.Rule('all', True, ['rtems48i'])}

# RTEMS 4.10
${template.Rule('rtems48i', True, None)}
${'\t'}if [ ! -d "$(RTEMS_BUILD_DIR)" ]; then ${'\\'}
${'\t'}$(MKDIR) $(RTEMS_BUILD_DIR) && ${'\\'}
${'\t'}$(MKDIR) ${base_dir}/edilib && ${'\\'}
${'\t'}cd $(RTEMS_BUILD_DIR); $(CURRENT_PATH)/rtems48i/configure ${'\\'}
${'\t\t'}--target=$(RTEMS_IMPR_TARGET) ${'\\'}
${'\t\t'}--enable-rtems-inlines ${'\\'}
${'\t\t'}--disable-itron ${'\\'}
${'\t\t'}--disable-multiprocessing ${'\\'}
${'\t\t'}--disable-posix ${'\\'}
${'\t\t'}--disable-networking ${'\\'}
${'\t\t'}--disable-cxx ${'\\'}
${'\t\t'}--disable-tests ${'\\'}
${'\t\t'}--enable-rtemsbsp=$(RTEMS_IMPR_BSP) ${'\\'}
${'\t\t'}--prefix=$(RTEMS_INSTALL_DIR) && cd ..; ${'\\'}
${'\t'}fi
${'\t'}make -C $(RTEMS_BUILD_DIR) CPPFLAGS='$(AIR_HEADERS)' && ${'\\'}
${'\t'}make -C $(RTEMS_BUILD_DIR) install && ${'\\'}
${'\t'}make -C edilib BUILD_DIR='$(EDILIB_BUILD_DIR)'

# Clean
${template.Rule('clean', True, None)}
${template.Remove(base_dir)}

# Distclean
${template.Rule('distclean', True, None)}
${template.Remove(base_dir)}
${template.Remove(output_file)}


