##
# @package air.libraries.libiop
# @author pfnf
# @brief libiop configuration

__author__ = 'pfnf'

import os
import utils
from air import *
import utils.file as file_tools

# @brief Name
name = "libiop"

# @brief Description
description = "I/O Partition functionality"

# @brief Library Source files
#source_files = []               # empty, the magic happens in the makefile template
source_files = set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_LIBS_DIRECTORY, 'libiop'), ['.c']),
]))
# @brief Library Header files
#header_files = []               # empty, the magic happens in the makefile template
header_files = set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_LIBS_DIRECTORY, 'libiop'), ['.h']),
]))
# @brief Library dependencies
dependencies = ['libprintf']

# @brief Custom Makefile
#makefile = os.path.join(LIBRARIES_DIRECTORY, 'libiop', 'iop_makefile.mako')
makefile = None

# @brief Requires POS
requires_pos = None
