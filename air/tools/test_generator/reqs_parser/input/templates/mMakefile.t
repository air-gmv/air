# System Paths 			########################################################

% if editest:
DIR_LEVEL=../../../
% else:
DIR_LEVEL=../../
% endif

export BASE_DIR=$(abspath $(shell pwd)/$(DIR_LEVEL))
include $(DIR_LEVEL)/air.mk

include $(AIR_PART_RTEMS)/$(AIR_ARCH)/$(AIR_TARGET)/Makefile.inc

include $(RTEMS_CUSTOM)
include $(RTEMS_ROOT)/make/directory.cfg


SUBDIRS=${folders}

