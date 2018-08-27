<%
    import os
    import air
    import utils.templates as makoutils
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("AIR OS - Makefile")}

export AIR_ROOT=${air.ROOT_DIRECTORY}
export AIR_INSTALL=${air.INSTALL_DIRECTORY}
export AIR_PMK=$(AIR_INSTALL)/${air.PMK_DIR_NAME}
export AIR_POS=$(AIR_INSTALL)/${air.POS_DIR_NAME}
export AIR_LIBS=$(AIR_INSTALL)/${air.LIBS_DIR_NAME}
export AIR_TOOLS=$(AIR_INSTALL)/${air.TOOLS_DIR_NAME}

# Makefile Include file
${template.MakefileInc()}

# All
${template.Rule('all', True, [ 'pmk', 'pos', 'libs', 'tools' ])}

# No Configure
${template.Rule('noconf', True, [ 'pmk', 'posnc', 'libs', 'tools' ])}

# PMK
${template.Rule('pmk', True, None)}
${template.RunMakeInDir(air.PMK_DIR_NAME, 'all')}

# POS
${template.Rule('pos', True, [ 'pmk' ])}
${template.RunMakeInDir(air.POS_DIR_NAME, 'all')}

# POS No Configure
${template.Rule('posnc', True, [ 'pmk' ])}
${template.RunMakeInDir(air.POS_DIR_NAME, 'noconf')}

# Libraries
${template.Rule('libs', True, [ 'pmk', 'pos' ])}
${template.RunMakeInDir(air.LIBS_DIR_NAME, 'all')}

# Tools
${template.Rule('tools', True, None)}
${template.RunMakeInDir(os.path.join(air.TOOLS_DIR_NAME, 'partition_assembler'), 'all')}
${template.SymLink(os.path.join('..', '..', 'tools', 'configurator', 'configurator.py'), os.path.join('$(AIR_TOOLS)', 'configurator'))}

# Clean
${template.Rule('clean', True, None)}
${template.Remove(os.path.join('$(AIR_TOOLS)', 'configurator'))}
${template.RunMakeInDir(air.LIBS_DIR_NAME, 'clean')}
${template.RunMakeInDir(air.POS_DIR_NAME, 'clean')}
${template.RunMakeInDir(air.PMK_DIR_NAME, 'clean')}
${template.RunMakeInDir(os.path.join(air.TOOLS_DIR_NAME, 'partition_assembler'), 'clean')}
${template.Remove(air.INSTALL_DIRECTORY)}

# Distclean
${template.Rule('distclean', True, None)}
${template.RunMakeInDir(air.LIBS_DIR_NAME, 'distclean')}
${template.RunMakeInDir(air.POS_DIR_NAME, 'distclean')}
${template.RunMakeInDir(air.PMK_DIR_NAME, 'distclean')}
${template.RunMakeInDir(os.path.join(air.TOOLS_DIR_NAME, 'partition_assembler'), 'distclean')}
${template.Remove(air.INSTALL_DIRECTORY)}
${template.Remove('Makefile')}
${template.Remove('Makefile.inc')}
${template.Remove('.air_config')}


