<%
    import os
    import xky
    import utils.templates as makoutils
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("XKY OS - Makefile")}

export XKY_ROOT=${xky.ROOT_DIRECTORY}
export XKY_INSTALL=${xky.INSTALL_DIRECTORY}
export XKY_PMK=$(XKY_INSTALL)/${xky.PMK_DIR_NAME}
export XKY_POS=$(XKY_INSTALL)/${xky.POS_DIR_NAME}
export XKY_LIBS=$(XKY_INSTALL)/${xky.LIBS_DIR_NAME}
export XKY_TOOLS=$(XKY_INSTALL)/${xky.TOOLS_DIR_NAME}

# Makefile Include file
${template.MakefileInc()}

# All
${template.Rule('all', True, [ 'pmk', 'pos', 'libs', 'tools' ])}

# PMK
${template.Rule('pmk', True, None)}
${template.RunMakeInDir(xky.PMK_DIR_NAME, 'all')}

# POS
${template.Rule('pos', True, [ 'pmk' ])}
${template.RunMakeInDir(xky.POS_DIR_NAME, 'all')}

# Libraries
${template.Rule('libs', True, [ 'pmk', 'pos' ])}
${template.RunMakeInDir(xky.LIBS_DIR_NAME, 'all')}

# Tools
${template.Rule('tools', True, None)}
${template.RunMakeInDir(os.path.join(xky.TOOLS_DIR_NAME, 'partition_assembler'), 'all')}
${template.SymLink(os.path.join('..', '..', 'tools', 'configurator', 'configurator.py'), os.path.join('$(XKY_TOOLS)', 'configurator'))}

# Clean
${template.Rule('clean', True, None)}
${template.Remove(os.path.join('$(XKY_TOOLS)', 'configurator'))}
${template.RunMakeInDir(xky.LIBS_DIR_NAME, 'clean')}
${template.RunMakeInDir(xky.POS_DIR_NAME, 'clean')}
${template.RunMakeInDir(xky.PMK_DIR_NAME, 'clean')}
${template.RunMakeInDir(os.path.join(xky.TOOLS_DIR_NAME, 'partition_assembler'), 'clean')}
${template.Remove(xky.INSTALL_DIRECTORY)}

# Distclean
${template.Rule('distclean', True, None)}
${template.RunMakeInDir(xky.LIBS_DIR_NAME, 'distclean')}
${template.RunMakeInDir(xky.POS_DIR_NAME, 'distclean')}
${template.RunMakeInDir(xky.PMK_DIR_NAME, 'distclean')}
${template.RunMakeInDir(os.path.join(xky.TOOLS_DIR_NAME, 'partition_assembler'), 'distclean')}
${template.Remove(xky.INSTALL_DIRECTORY)}
${template.Remove('Makefile')}
${template.Remove('Makefile.inc')}
${template.Remove('.xky_config')}


