#
#  $Id: Makefile,v 1.4 2011/07/27 12:03:59 jaac Exp $
#
# Templates/Makefile.leaf
# 	Template leaf node Makefile
#

#	AIR_TARGET should be defined as an environment variable
#	e.g.	export AIR_TARGET=i386-rtems4.8/pc386	
#			export AIR_TARGET=sparc-rtems4.8/leon2

#	RTEMS_AIR should be defined as an environment variable
#	e.g.	export RTEMS_AIR=/home/user/MY-AIR/MY-RTEMS-PARTITION-INSTALLATION
#			export RTEMS_AIR=/opt/rtems-4.8-partition	

# System Paths 			########################################################
% if editest:
DIR_LEVEL=../../../..
% else:
DIR_LEVEL=../../..
% endif

export BASE_DIR=$(abspath $(shell pwd)/$(DIR_LEVEL))
include $(DIR_LEVEL)/air.mk

# user defined variables #######################################################
PART_NUMBER=P${part_num}
MAKE_MARKER=$(ARCH)/${part_name}.made

# C source names, if any, go here -- minus the .c 
# any file included in the partition root : to change this behaviour this 
# line must be changed
C_PIECES=$(basename $(wildcard *.c))

# user defined variables ########################################## END   ######

# Sub-systems Paths ############################################################
# pal vars
PMK_SRC_PATH=$(PMK_ROOT)/src/pmk
PMK_LIB_PATH=$(PMK_ROOT)/lib
PAL_SRC_PATH=$(PMK_ROOT)/src/pal
PAL_LIB_PATH=$(PMK_ROOT)/lib

SRC_PATHS=$(PMK_SRC_PATH) $(PAL_SRC_PATH)
LIB_PATHS=$(PMK_LIB_PATH) $(PAL_LIB_PATH)

AIRPEX_LIBS=
% if apex_libs:
% for apex_lib in apex_libs:
AIRPEX_LIBS+=$(AIRPEX_ROOT)/lib/${apex_lib}
% endfor
APX_SRC_PATH=$(AIRPEX_ROOT)/include
APX_LIB_PATH=$(AIRPEX_ROOT)/lib
SRC_PATHS+=$(APX_SRC_PATH)
LIB_PATHS+=$(APX_LIB_PATH)
% endif

% if iop_libs:
SRC_PATHS+=$(IOP_ROOT)/src/iop
LIB_PATHS+=$(IOP_ROOT)/lib
% endif

% if mw_libs:
SRC_PATHS+=$(IOP_ROOT)/src/iop $(IOP_ROOT)/src/mw
LIB_PATHS+=$(IOP_ROOT)/lib
% endif

RENAMED_OBJS_PATH=renamed

# Generic rules ###########################################
C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=$(ARCH)/%.o)

SRCS=$(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS=$(C_O_FILES) $(CC_O_FILES) $(S_O_FILES)

#  List of RTEMS Classic API Managers to be included in the application
#  goes here. Use:
#     MANAGERS=all
# to include all RTEMS Classic API Managers in the application or
# something like this to include a specific set of managers.
#     MANAGERS=io event message rate_monotonic semaphore timer
MANAGERS=all

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(RTEMS_ROOT)/make/leaf.cfg

#
# (OPTIONAL) Add local stuff here using +=
#

DEFINES  +=
CPPFLAGS +=-I./ $(addprefix -I,$(SRC_PATHS))
% if test in EDI_TESTS:
CPPFLAGS +=-I../edisupport/include
CFLAGS += -DBSP_leon3 -fno-builtin-printf
% endif

#
# CFLAGS_DEBUG_V are used when the `make debug' target is built.
# To link your application with the non-optimized RTEMS routines,
# uncomment the following line:
# CFLAGS_DEBUG_V += -qrtems_debug
#

LD_PATHS  += 
LD_LIBS   += 
LDFLAGS   += -Wl,-r $(addprefix -L,$(LIB_PATHS))

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += $(MAKE_MARKER) $(RENAMED_OBJS_PATH)/*.o $(RENAMED_OBJS_PATH)
CLOBBER_ADDITIONS +=

# TARGETS #####################################################################
all:	$(ARCH) $(SRCS) $(MAKE_MARKER)

# Debug Makefile Data
ID=($(PART_NUMBER))

<% newline = "\\" %>
# RULES / MAKE ################################################################
$(MAKE_MARKER): $(OBJS)
	mkdir -p $(RENAMED_OBJS_PATH)
% if apex_libs:
	<% apex_libs_str = " ".join(["$(AIRPEX_ROOT)/lib/" + x for x in apex_libs]) %>
	#$(ID) get defined symbols from AIRPEX libraries and rename them to $(PART_NUMBER)symbol in $(PART_NUMBER).ren
	sparc-rtems4.8-nm -A ${apex_libs_str}	 			${newline}
		| grep -v " U " 								${newline}
		| awk '{print ""$$3" $(PART_NUMBER)"$$3""}' 	${newline}
		| sort -u >  $(ARCH)/$(PART_NUMBER).ren
		
	% for apex_lib in apex_libs:
	#$(ID) Make a local copy with the symbols redefined for $(PART_NUMBER)
	#$(ID) ${apex_lib}  -> $(RENAMED_OBJS_PATH)/$(PART_NUMBER)${apex_lib}
	@sparc-rtems4.8-objcopy --redefine-syms $(ARCH)/$(PART_NUMBER).ren \
		$(AIRPEX_ROOT)/lib/${apex_lib}	 $(RENAMED_OBJS_PATH)/$(PART_NUMBER)${apex_lib}
	% endfor
% else:

	@cat /dev/null > $(ARCH)/$(PART_NUMBER).ren
% endif
% if iop_libs:
	<% iop_libs_str = " ".join(["$(IOP_ROOT)/lib/" + x for x in iop_libs]) %>
	#$(ID) get defined symbols from IOP libraries and rename them to $(PART_NUMBER)symbol in $(PART_NUMBER).ren
	sparc-rtems4.8-nm -A ${iop_libs_str}	 			${newline}
		| grep -v " U " 								${newline}
		| awk '{print ""$$3" $(PART_NUMBER)"$$3""}' 	${newline}
		| sort -u >>  $(ARCH)/$(PART_NUMBER).ren
% endif
% if mw_libs:
	<% mw_libs_str = " ".join(["$(IOP_ROOT)/lib/" + x for x in mw_libs]) %>
	#$(ID) get defined symbols from IOP libraries and rename them to $(PART_NUMBER)symbol in $(PART_NUMBER).ren
	sparc-rtems4.8-nm -A ${mw_libs_str}	 		     	${newline}
		| grep -v " U " 								${newline}
		| awk '{print ""$$3" $(PART_NUMBER)"$$3""}' 	${newline}
		| sort -u >>  $(ARCH)/$(PART_NUMBER).ren
% endif
	
	#$(ID) Get defined symbols in this partition object files (into o-optimize/$(PART_NUMBER).ren)
	@sparc-rtems4.8-nm -A $(ARCH)/*.o 					${newline}
		| grep -v " U " 								${newline}
% if test_symbs:
		% for test_symbol in test_symbs:
		| grep -v "${test_symbol}"						${newline}
		% endfor
% endif
		| awk '{print ""$$3" $(PART_NUMBER)"$$3""}' 	${newline}
		| sort -u >> $(ARCH)/$(PART_NUMBER).ren
% if iop_libs:
	% for iop_lib in iop_libs:
	#$(ID) Make a local copy with the symbols redefined for $(PART_NUMBER)
	#$(ID) ${iop_lib}  -> $(RENAMED_OBJS_PATH)/$(PART_NUMBER)${iop_lib}
	@sparc-rtems4.8-objcopy --redefine-syms $(ARCH)/$(PART_NUMBER).ren \
		$(IOP_ROOT)/lib/${iop_lib}	 $(RENAMED_OBJS_PATH)/$(PART_NUMBER)${iop_lib}
	% endfor
% endif
% if mw_libs:		
	% for mw_lib in mw_libs:
	#$(ID) Make a local copy with the symbols redefined for $(PART_NUMBER)
	#$(ID) ${mw_lib}  -> $(RENAMED_OBJS_PATH)/$(PART_NUMBER)${mw_lib}
	@sparc-rtems4.8-objcopy --redefine-syms $(ARCH)/$(PART_NUMBER).ren \
		$(IOP_ROOT)/lib/${mw_lib}	 $(RENAMED_OBJS_PATH)/$(PART_NUMBER)${mw_lib}
	% endfor
% endif
	#$(ID) Rename symbols
	@python $(PMK_ROOT)/bin/multcom_prefix.py \
		"sparc-rtems4.8-objcopy --redefine-syms $(ARCH)/$(PART_NUMBER).ren" $(RENAMED_OBJS_PATH) $?
	#$(ID) Generate make marker
	@echo $? > $(MAKE_MARKER)