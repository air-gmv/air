##
# @package air.pos.rtems48i
# @author $(AIR_RUNNER_USER)
# @brief RTEMS 4.8 (impr) configuration

__author__ = '$(AIR_RUNNER_USER)'

import os
import utils
from air import *
import utils.file as file_tools

# @brief Name
name = "rtems48i"

# @brief Description
description = "RTEMS-4.8-IMPR"

# @brief OS alias
alias = ['rtems48i']

# @brief Supported git checkout id
git_id = ""


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
makefile = os.path.join(POS_DIRECTORY, 'rtems48i', 'rtems48i_makefile.mako')

# @brief List of lines to be add to Makefile.inc
makefile_inc = [
    "RTEMS_IMPR_DIRECTORY=rtems48i",
    "RTEMS_IMPR_TARGET=arm-rtems5",
    "RTEMS_IMPR_BSP=xilinx_zynq_artyz7",
]

# @brief List of application partition templates
partition_templates = [
    os.path.join(POS_DIRECTORY, 'rtems48i', 'rtems48i_partition_makefile.mako'),    # POS makefile
    os.path.join(POS_DIRECTORY, 'rtems48i', 'init.c.mako'),
    os.path.join(POS_DIRECTORY, 'rtems48i', 'rtems_config.h.mako'),
]
