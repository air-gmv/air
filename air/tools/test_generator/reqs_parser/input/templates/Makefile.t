#
#  $Id: Makefile.t,v 1.2 2011/09/07 11:03:14 jaac Exp $
#
# Templates/Makefile.leaf
# 	Template leaf node Makefile
#

# Target name ################################################################
EXEC=AIRPMK.exe
OUTPUT_DIR=../executable

# System Paths ################################################################
% if editest:
DIR_LEVEL=../../../..
% else: 
DIR_LEVEL=../../..
% endif

export BASE_DIR=$(abspath $(shell pwd)/$(DIR_LEVEL))
include $(DIR_LEVEL)/air.mk

# RTEMS Lib paths
ifeq ($(shell uname -i),x86_64)
  OPT_LIB=/opt/rtems-4.8/lib64
else
  OPT_LIB=/opt/rtems-4.8/lib
endif

% if editest:
TEST_SUPPORT_FLAGS = ${test_support_flags} 
% endif

# Sub-systems Paths ############################################################
# sample dir
SAMPLE_PATH=$(abspath $(shell pwd)/..)

# pal vars
HAL_SRC_PATH=$(PMK_ROOT)/src/hal
HAL_LIB_PATH=$(PMK_ROOT)/lib
PMK_SRC_PATH=$(PMK_ROOT)/src/pmk
PMK_LIB_PATH=$(PMK_ROOT)/lib
PAL_SRC_PATH=$(PMK_ROOT)/src/pal
PAL_LIB_PATH=$(PMK_ROOT)/lib

SRC_PATHS=$(HAL_SRC_PATH) $(PMK_SRC_PATH) $(PAL_SRC_PATH)
LIB_PATHS=$(HAL_LIB_PATH) $(PMK_LIB_PATH) $(PAL_LIB_PATH)

# sample DIR (one for each sample system)
APP_PATH=$(abspath ..)
BASE_PATH=$(abspath ../../..)
# PMK / Partition 0 init section
INIT=

RENAMED_OBJS_PATH=renamed

# Partition Names #############################################################
PART_NAMES=${part_names}

# Per partition libraries #####################################################
PART_LIBS=

# for each partition, check what libraries need to be included
% for p in range(0,number_of_partitions):
	% if apex_libs[p]:
		% for lib in apex_libs[p]:
PART_LIBS += $(SAMPLE_PATH)/${part_folders[p]}/$(RENAMED_OBJS_PATH)/P${p}${lib}
		% endfor
	% endif
	% if io_libs[p]:
		% for lib in io_libs[p]:
PART_LIBS += $(SAMPLE_PATH)/${part_folders[p]}/$(RENAMED_OBJS_PATH)/P${p}${lib}
		% endfor
	% endif
% endfor


# Global Libs ##################################################################
GLOB_LIBS=

# Source files		############################################################
# C source names, if any, go here -- minus the .c
C_PIECES=$(basename $(wildcard *.c))
C_FILES=$(C_PIECES:%=%.c)
C_O_FILES=$(C_PIECES:%=$(ARCH)/%.o)

# C++ source names, if any, go here -- minus the .cc
CC_PIECES=
CC_FILES=$(CC_PIECES:%=%.cc)
CC_O_FILES=$(CC_PIECES:%=$(ARCH)/%.o)

H_FILES=

# Assembly source names, if any, go here -- minus the .S
S_PIECES=
S_FILES=$(S_PIECES:%=%.S)
S_O_FILES=$(S_FILES:%.S=$(ARCH)/%.o)

SRCS=$(C_FILES) $(CC_FILES) $(H_FILES) $(S_FILES)
OBJS=$(C_O_FILES) $(CC_O_FILES) $(S_O_FILES)

# partition specific objects (all *.o)
PART_PATHS=$(addsuffix /$(RENAMED_OBJS_PATH),$(addprefix $(APP_PATH)/,$(PART_NAMES)))
PART_OBJS=$(addsuffix /*.o,$(PART_PATHS))

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

# Target name
PGMS=$(OUTPUT_DIR)/$(ARCH)/$(EXEC)

#
# (OPTIONAL) Add local stuff here using +=
#

DEFINES  +=
CPPFLAGS +=
CFLAGS   +=-I./ $(addprefix -I,$(SRC_PATHS)) -fno-builtin-printf

#
# CFLAGS_DEBUG_V are used when the `make debug' target is built.
# To link your application with the non-optimized RTEMS routines,
# uncomment the following line:
# CFLAGS_DEBUG_V += -qrtems_debug
#

LD_PATHS  +=
LD_LIBS   += 	

#
# Add your list of files to delete here.  The config files
#  already know how to delete some stuff, so you may want
#  to just run 'make clean' first to see what gets missed.
#  'make clobber' already includes 'make clean'
#

CLEAN_ADDITIONS += $(ARCH)/usr_*.o
CLEAN_ADDITIONS += $(ARCH)/libgcc.a
CLEAN_ADDITIONS += linkcmds.leon2
CLEAN_ADDITIONS +=$(OUTPUT_DIR)/$(ARCH)
CLOBBER_ADDITIONS +=

# rename symbols macros
PREFIX_PMK=sparc-rtems4.8-objcopy --prefix-symbols pmk_

all: $(ARCH) $(PGMS)


# MAIN ########################################################################
$(PGMS): $(OBJS) $(SRCS) $(PART_OBJS) $(GLOB_LIBS) $(PART_LIBS) $(PMK_LIB_PATH)/pmk.a $(PMK_LIB_PATH)/pal_rtems.a
	@mkdir -p $(OUTPUT_DIR)
	@mkdir -p $(OUTPUT_DIR)/$(ARCH)
	# rename symbols from gcc lib to pmk
	@$(PREFIX_PMK) $(OPT_LIB)/gcc/sparc-rtems4.8/4.2.4/v8/libgcc.a 	$(ARCH)/libgcc.a	
	
	# convert relative file references in linkcmds to absolute references
	@sed "s/\[AIR\_ROOT\]/$(subst /,\/,$(BASE_PATH))/" linkcmds		 	${"\\"}
		| sed "s/\[PMK\_ROOT\]/$(subst /,\/,$(PMK_ROOT))/"				${"\\"}
		| sed "s/\[AIR\_PART\_RTEMS\]/$(subst /,\/,$(AIR_PART_RTEMS))/"	${"\\"}
		| sed "s/\[SAMPLE\_ROOT\]/$(subst /,\/,$(SAMPLE_PATH))/"			${"\\"}
		> linkcmds.leon2
	
	# Link Everything
	$(LINK.c) $(AM_CFLAGS) $(AM_LDFLAGS)-Wl,-Map -Wl,$(OUTPUT_DIR)/fich  ${"\\"}
		## test system
		% if editest:
		$(TEST_SUPPORT_FLAGS)		${"\\"}
		% endif
		-qnolinkcmds -Tlinkcmds.leon2			${"\\"}
		-nostdlib -nodefaultlibs -nostartfiles 	${"\\"}
		-z muldefs -g 				${"\\"}
		-e pmk_trap_table 			${"\\"}
		-o $(basename $@).exe 		${"\\"}
		$(INIT) 					${"\\"}
		$(GLOB_LIBS)  				${"\\"}
		$(PART_OBJS) 				${"\\"}
		$(PART_LIBS) 				${"\\"}
		$(LINK_OBJS)				${"\\"}
		-Wl,--start-group 			${"\\"}
		## PMK Libs
		% for pmklib in pmk_libs :
		${pmklib}					${"\\"}
		% endfor	
		## PAL Libs
		% for pallib in pal_libs :
		${pallib}					${"\\"}
		% endfor
		## Test Libs
		% for testlib in test_libs :
		${testlib}					${"\\"}
		% endfor
		## Gcc Lib
									${"\\"}
		$(ARCH)/libgcc.a 			${"\\"}
									${"\\"}
		${"\\"}
		## RTEMSIMPR specific RTEMS Libs
		% for poslib in pos_libs :
		${poslib}					${"\\"}
		% endfor
		## APEX requires libgcc
		% if apex_libs:
		$(OPT_LIB)/gcc/sparc-rtems4.8/4.2.4/v8/libgcc.a   ${"\\"}
		% endif
		## end group
		-Wl,--end-group
		
	# generate tsim compatible executable
	@$(OBJCOPY) -O srec 			${"\\"}
		--remove-section=.rodata 	${"\\"}
		--remove-section=.comment 	${"\\"}
		--remove-section=.note 		${"\\"}
		$(basename $@).exe $(basename $@)2.exe	
	# Done. All is good.
