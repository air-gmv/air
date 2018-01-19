<%
    import air
    import utils.templates as makoutils
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("AIR OS - Directory Makefile")}\

# All
${template.Rule('all', True, None)}
%for i, directory in enumerate(directories):
${template.RunMakeInDir(directory, 'all')}
%endfor

# Clean
${template.Rule('clean', True, None)}
%for i, directory in enumerate(directories):
${template.RunMakeInDir(directory, 'clean')}
%endfor

# Distclean
${template.Rule('distclean', True, None)}
%for i, directory in enumerate(directories):
${template.RunMakeInDir(directory, 'distclean')}
%endfor
${template.Remove(output_file)}

