##
# @package air.pos.rtems5
# @author $(AIR_RUNNER_USER)
# @brief RTEMS 4.12 configuration

__author__ = '$(AIR_RUNNER_USER)'

import os
import utils
from air import *
import utils.file as file_tools

# @brief Name
name = "rtems5"

# @brief Description
description = "RTEMS5"

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
makefile = os.path.join(POS_DIRECTORY, 'rtems5', 'rtems5_makefile.mako')

# @brief List of lines to be add to Makefile.inc
makefile_inc = [
    "RTEMS_DIRECTORY=rtems5",
    "RTEMS_TARGET=sparc-rtems5",
    "RTEMS_BSP=leon3",
]

# @brief List of application partition templates
partition_templates = [
    os.path.join(POS_DIRECTORY, 'rtems5', 'rtems5_partition_makefile.mako'),    # POS makefile
    os.path.join(POS_DIRECTORY, 'rtems5', 'init.c.mako'),
    os.path.join(POS_DIRECTORY, 'rtems5', 'rtems_config.h.mako'),
]
