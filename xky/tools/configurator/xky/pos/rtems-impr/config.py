##
# @package xky.pos.rtems-impr
# @author $(AIR_RUNNER_USER)
# @brief RTEMS 4.12 configuration

__author__ = '$(AIR_RUNNER_USER)'

import os
import utils
from xky import *
import utils.file as file_tools

# @brief Name
name = "rtems-impr"

# @brief Description
description = "RTEMS-IMPR"

# @brief OS alias
alias = ['rtems']

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
makefile = os.path.join(POS_DIRECTORY, 'rtems-impr', 'rtems-impr_makefile.mako')

# @brief List of lines to be add to Makefile.inc
makefile_inc = [
    "RTEMS_DIRECTORY=rtems-impr",
    "RTEMS_TARGET=sparc-rtems5",
    "RTEMS_BSP=leon3",
]

# @brief List of application partition templates
partition_templates = [
    os.path.join(POS_DIRECTORY, 'rtems-impr', 'rtems-impr_partition_makefile.mako'),    # POS makefile
    os.path.join(POS_DIRECTORY, 'rtems-impr', 'init.c.mako'),
    os.path.join(POS_DIRECTORY, 'rtems-impr', 'rtems_config.h.mako'),
]
