<%
    import os
    import air
    import utils
    import utils.file as fileutils
    import utils.templates as makoutils
%>

<%def name="MakefileInc(in_app=False)">\
<%
    import os
    import air
%>\
%if not in_app:
include ${os.path.join(os.path.relpath(air.ROOT_DIRECTORY, output_dir), 'Makefile.inc')} \
%else:
include $(AIR_ROOT)/Makefile.inc
%endif
</%def>

<%def name="RTEMSMakefileInclude()">\
<%
    import os
    import air
%>\
include ${os.path.join(os.path.relpath(air.INSTALL_DIRECTORY, output_dir), 'rtems.inc')}\
</%def>


<%def name="FileHeader(purpose)">\
<%
        import configurator
%>\
#
# Makefile : ${purpose}
# Generated by AIR Configurator Tool v${configurator.__version__}
#\
</%def>

<%def name="DefaultTools()">\
CD=cd
CP=cp -ru
RM=rm -rf
MKDIR=mkdir -p
ECHO=echo -e
MV=mv
SYMLINK=ln -s
TESTD=test -d\
</%def>

<%def name="LibraryIncludes(libraries)">\
% for i, libname in enumerate(libraries):
-B$(AIR_LIBS)/${libname.lower()}/${'\\' if i < len(libraries) - 1 else ''}
% endfor \
</%def>

<%def name="LibraryDependencyIncludes(os_configuration, lib, output_dir)">\
<%
    import os
    import air

    include_dirs = os_configuration.getLibraryDependenciesIncludes(lib)
    include_dirs = map(lambda x: os.path.join(air.INSTALL_LIBS_DIRECTORY, x), include_dirs)
    include_dirs.append(output_dir)
%>\
-I./${'\\'}
% for i, directory in enumerate(include_dirs):
-B${os.path.relpath(directory, output_dir)}${'\\' if i < len(include_dirs) - 1 else ''}
% endfor
</%def>

<%def name="FileList(file_list)">\
% for i, file_name in enumerate(file_list):
${file_name}${'\\' if i < len(file_list) - 1 else ''}
% endfor \
</%def>

<%def name="HeaderFilesList(directory, os_configuration=None)">\
<%
    import os
    import utils.file as fileutils
    include_files = fileutils.getFilesByExtensions(directory, [ '.h', '.ld' ])

    # apply common path
    include_files = [ os.path.relpath(inc_file, directory) for inc_file in include_files ]
%>\
% for i, include_file in enumerate(include_files):
${include_file}${'\\' if i < len(include_files) - 1 else ''}
% endfor
</%def>

<%def name="Rule(name, phony, dependencies)">\
% if phony:
.PHONY : ${name}
% endif
% if dependencies is not None:
${name}: ${' '.join(dependencies)}\
% else:
${name}:\
% endif
</%def>

<%def name="MakeLibrary(lib, objs, for_host=False)">\
%if for_host:
${'\t'}$(call make-host-library, ${lib}, ${objs})\
%else:
${'\t'}$(call make-target-library, ${lib}, ${objs})\
%endif
</%def>

<%def name="SymLink(file, target)">\
${'\t'}@($(RM) ${target})
${'\t'}@($(SYMLINK) ${file} ${target})\
</%def>

<%def name="Remove(files)">\
${'\t'}@($(RM) ${files})\
</%def>

<%def name="RunMakeInDir(recurse, dir, rule)">\
<%
    import os
%>\
% if os.path.isdir(os.path.join(output_dir, dir)) and (os.path.isfile(os.path.join(output_dir, dir, 'Makefile') or  os.path.isfile(os.path.join(output_dir, dir, 'makefile')))):
% if recurse:
% if rule is not None:
${'\t'}$(MAKE) -C ${dir} ${rule}\
% else:
${'\t'}$(MAKE) -C ${dir}\
% endif
% else:
% if rule is not None:
${'\t'}make -C ${dir} ${rule}\
% else:
${'\t'}make -C ${dir}\
% endif
% endif
% endif
</%def>

<%def name="CopyTo(files, destination)">\
${'\t'}@($(TESTD) ${destination} || $(MKDIR) ${destination})
${'\t'}@$(foreach obj,${files},$(CP) $(obj) ${destination}/$(dirname $(obj));)\
</%def>

<%def name="get_headers_directories(headers)">\
<%
    import os
    headers = set([os.path.dirname(d) for d in headers])
%>\
% for i, directory in enumerate(headers):
-I${directory}${'\\' if i < len(headers) - 1 else ''}
% endfor
</%def>
