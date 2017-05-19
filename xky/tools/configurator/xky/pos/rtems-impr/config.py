##
# @package xky.pos.rtems-impr
# @author pfnf
# @brief RTEMS 4.8 IMPR configuration

__author__ = 'pfnf'

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
    "RTEMS_TARGET=sparc-rtems4.8",
    "RTEMS_BSP=leon3",
    "RTEMS_EXTERNAL_LIBRARY={0}".format(os.path.join('edilib', 'library'))
]

# @brief List of application partition templates
partition_templates = [
    os.path.join(POS_DIRECTORY, 'rtems-impr', 'rtems-impr_partition_makefile.mako'),    # POS makefile
    os.path.join(POS_DIRECTORY, 'rtems-impr', 'init.c.mako'),
    os.path.join(POS_DIRECTORY, 'rtems-impr', 'rtems_config.h.mako'),
]
