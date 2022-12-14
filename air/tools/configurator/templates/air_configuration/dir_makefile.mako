<%
    import air
    import utils.templates as makoutils
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("AIR OS - Directory Makefile")}\

MAKEFLAGS += -j4

# All
${template.Rule('all', True, directories)}
%for i, directory in enumerate(directories):
${template.Rule(directory, True, None)}
${template.RunMakeInDir(True, directory, None)}
%endfor

# No Configure
${template.Rule('noconf', True, None)}
%for i, directory in enumerate(directories):
${template.RunMakeInDir(True, directory, 'noconf')}
%endfor

# Clean
${template.Rule('clean', True, None)}
%for i, directory in enumerate(directories):
${template.RunMakeInDir(True, directory, 'clean')}
%endfor

# Distclean
${template.Rule('distclean', True, None)}
%for i, directory in enumerate(directories):
${template.RunMakeInDir(True, directory, 'distclean')}
%endfor
${template.Remove(output_file)}

