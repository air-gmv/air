<%
    import os
    import air
    import utils.templates as makoutils
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("AIR application Makefile")}\

# AIR OS installation directories
export AIR_ROOT=${air.ROOT_DIRECTORY}
export AIR_INSTALL=${air.INSTALL_DIRECTORY}
export AIR_PMK=$(AIR_INSTALL)/${air.PMK_DIR_NAME}
export AIR_POS=$(AIR_INSTALL)/${air.POS_DIR_NAME}
export AIR_LIBS=$(AIR_INSTALL)/${air.LIBS_DIR_NAME}
export AIR_TOOLS=$(AIR_INSTALL)/${air.TOOLS_DIR_NAME}

# Makefile Include file
${template.MakefileInc(True)}

# All
${template.Rule('all', True, None)}
${template.RunMakeInDir(True, 'common', None)}
% for i, partition in enumerate(app_configuration.partitions):
${template.RunMakeInDir(True, partition.directory, 'all')}
% endfor
${template.RunMakeInDir(True, 'config', 'all')}

# Clean
${template.Rule('clean', True, None)}
${template.RunMakeInDir(True, 'common', 'clean')}
% for i, partition in enumerate(app_configuration.partitions):
${template.RunMakeInDir(True, partition.directory, 'clean')}
% endfor
${template.RunMakeInDir(True, 'config', 'clean')}

# Distclean
${template.Rule('distclean', True, ['clean'])}
${template.RunMakeInDir(True, 'config', 'distclean')}
${'\t'}${os.path.join('$(AIR_TOOLS)', 'configurator clean')}

