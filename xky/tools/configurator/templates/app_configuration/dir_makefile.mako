<%
    import os
    import xky
    import utils.templates as makoutils
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("XKY application Makefile")}\

# XKY OS installation directories
export XKY_ROOT=${xky.ROOT_DIRECTORY}
export XKY_INSTALL=${xky.INSTALL_DIRECTORY}
export XKY_PMK=$(XKY_INSTALL)/${xky.PMK_DIR_NAME}
export XKY_POS=$(XKY_INSTALL)/${xky.POS_DIR_NAME}
export XKY_LIBS=$(XKY_INSTALL)/${xky.LIBS_DIR_NAME}
export XKY_TOOLS=$(XKY_INSTALL)/${xky.TOOLS_DIR_NAME}

# Makefile Include file
${template.MakefileInc(True)}

# All
${template.Rule('all', True, None)}
${template.RunMakeInDir('common', None)}
% for i, partition in enumerate(app_configuration.partitions):
${template.RunMakeInDir(partition.directory, 'all')}
% endfor
${template.RunMakeInDir('config', 'all')}

# Clean
${template.Rule('clean', True, None)}
${template.RunMakeInDir('common', 'clean')}
% for i, partition in enumerate(app_configuration.partitions):
${template.RunMakeInDir(partition.directory, 'clean')}
% endfor
${template.RunMakeInDir('config', 'clean')}

# Distclean
${template.Rule('distclean', True, ['clean'])}
${template.RunMakeInDir('config', 'distclean')}
${'\t'}${os.path.join('$(XKY_TOOLS)', 'configurator clean')}

