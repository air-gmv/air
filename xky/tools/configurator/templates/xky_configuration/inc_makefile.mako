<%
	import os
	import xky
	import utils.templates as makoutils

	compiler = os_configuration.get_target_compiler()
%>\
<%namespace name="template" file="/makefile.mako"/>\
${template.FileHeader("XKY OS - Makefile")}

# Default tools
${template.DefaultTools()}

# Host compiler
HOST_CC=gcc
HOST_CXX=g++
HOST_LD=ld
HOST_AR=ar
HOST_RANLIB=ranllib

# Host compilation flags
HOST_CFLAGS = -g
HOST_CPPFLAGS = -g -Wall
HOST_CXXFLAGS = -g
HOST_LDFLAGS = -g
HOST_ARFLAGS =

# Target compiler
TARGET_CC=${compiler['CC']}
TARGET_CXX=${compiler['CXX']}
TARGET_LD=${compiler['LD']}
TARGET_AR=${compiler['AR']}
TARGET_RANLIB=${compiler['RANLIB']}

# Target compilation flags
TARGET_CFLAGS=${compiler['CFLAGS']}
TARGET_CPPFLAGS=${compiler['CPPFLAGS']}
TARGET_CXXFLAGS=${compiler['CXXFLAGS']}
TARGET_LDFLAGS=${compiler['LDFLAGS']}
TARGET_ARFLAGS=${compiler['ARFLAGS']}

# Rule to compile C source for HOST
$(HOST_BUILD)/%.o: %.c
	@($(TESTD) $(dir $@) || $(MKDIR) $(dir $@))
	$(HOST_CC) $(HOST_CPPFLAGS) $(HOST_CFLAGS) -c $< -o $@

# Rule to compile C++ source for HOST
$(HOST_BUILD)/%.o: %.cc
	@($(TESTD) $(dir $@) || $(MKDIR) $(dir $@))
	$(HOST_CXX) $(HOST_CPPFLAGS) $(HOST_CXXFLAGS) -c $< -o $@

# Rule to compile C++ source for HOST
$(HOST_BUILD)/%.o: %.cpp
	@($(TESTD) $(dir $@) || $(MKDIR) $(dir $@))
	$(HOST_CXX) $(HOST_CPPFLAGS) $(HOST_CXXFLAGS) -c $< -o $@

# Rule to compile C source for TARGET
$(TARGET_BUILD)/%.o: %.c
	@($(TESTD) $(dir $@) || $(MKDIR) -p $(dir $@))
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -c $< -o $@

# Rule to compile Assembly source for TARGET
$(TARGET_BUILD)/%.o: %.S
	@($(TESTD) $(dir $@) || $(MKDIR) -p $(dir $@))
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -DASM -c $< -o $@

# Rule to compile C++ source for TARGET
$(TARGET_BUILD)/%.o: %.cc
	@($(TESTD) $(dir $@) || $(MKDIR) -p $(dir $@))
	$(TARGET_CXX) $(TARGET_CPPFLAGS) $(TARGET_CXXFLAGS) -c $< -o $@

# Rule to compile C++ source for TARGET
$(TARGET_BUILD)/%.o: %.cpp
	@($(TESTD) $(dir $@) || $(MKDIR) -p $(dir $@))
	$(TARGET_CXX) $(TARGET_CPPFLAGS) $(TARGET_CXXFLAGS) -c $< -o $@

# Build library routine for HOST
define make-host-library
@($(ECHO) '\nCreating $(notdir $(1))\n')
@($(RM) $(1))
@($(TESTD) $(dir $(1)) || $(MKDIR) $(dir $(1)))
@($(HOST_AR) $(HOST_ARFLAGS) $(1) $(2))
@($(HOST_RANLIB) $(1))
endef

# Build library routine for TARGET
define make-target-library
@($(ECHO) '\nCreating $(notdir $(1))\n')
@($(RM) $(1))
@($(TESTD) $(dir $(1)) || $(MKDIR) $(dir $(1)))
@($(TARGET_AR) $(TARGET_ARFLAGS) $(1) $(2))
@($(TARGET_RANLIB) $(1))
endef

%for pos_name in os_configuration.get_supported_pos():
<%
	pos = os_configuration.get_pos_config(pos_name)
%>\
%if (pos.alias is None or pos_name not in pos.alias) and pos.makefile_inc is not None and len(pos.makefile_inc) > 0:
${'# {0} definitions'.format(pos.name)}
%for line in pos.makefile_inc:
${line}
%endfor

%endif
%endfor
