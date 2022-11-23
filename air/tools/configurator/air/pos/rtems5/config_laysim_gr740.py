##
# @package air.pos.rtems5
# @author $(AIR_RUNNER_USER)
# @brief RTEMS 5 configuration

__author__ = '$(AIR_RUNNER_USER)'

import os
import air
import utils
from air import *
import utils.file as file_tools
import air.configurations as air_configuration

# @brief Name
name = "rtems5"

# @brief Description
description = "RTEMS-5"

# @brief OS alias
alias = ['rtems5']

# @brief Supported git checkout id
git_id = "6bb9b3df7b5ea97e151d39654092c060d2175045"


# @brief POS Source files
source_files = {
    'sparc': [],             # empty, the magic happens in the makefile template
}

# @brief POS Private Header files
private_header_files = {
    'sparc': [],             # empty, the magic happens in the makefile template
}

# @brief POS Public Header files
public_header_files = {
    'sparc':  [],            # empty, the magic happens in the makefile template
}

# @brief Custom Makefile
makefile = os.path.join(POS_DIRECTORY, 'rtems5', 'rtems5_makefile.mako')

# @brief List of lines to be add to Makefile.inc
# RTEMS_DIRECTORY is  necessary only for lbiop
makefile_inc = [
    "RTEMS5_DIRECTORY=rtems5",
    "RTEMS5_TARGET=sparc-rtems5",
    "RTEMS5_BSP=gr740",
    "ifeq ($(RVS_ENABLED),1)",
    "TARGET_CC=/usr/local/pkg/RVS/v3.14/integration_resources/compiler_wrappers/gcc/gcc_wrap",
    "TARGET_LD=/usr/local/pkg/RVS/v3.14/integration_resources/compiler_wrappers/gcc/ld_wrap",
    "endif",
]


# @brief List of application partition templates
partition_templates = [
    os.path.join(POS_DIRECTORY, 'rtems5', 'rtems5_partition_makefile.mako'),    # POS makefile
    os.path.join(POS_DIRECTORY, 'rtems5', 'init.c.mako'),
    os.path.join(POS_DIRECTORY, 'rtems5', 'rtems_config.h.mako'),
]
