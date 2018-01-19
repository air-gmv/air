##
# @package air.libraries.libtest
# @author pfnf
# @brief libtest configuration

__author__ = 'pfnf'

import os
import utils
from air import *
import utils.file as file_tools

# @brief Name
name = "libtest"

# @brief Description
description = "Test support library"

# @brief OS alias
alias = ['rtems']

# @brief Library Source files
source_files = set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_LIBS_DIRECTORY, 'libtest'), ['.c']),
]))

# @brief Library Header files
header_files = set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_LIBS_DIRECTORY, 'libtest'), ['.h']),
]))

# @brief Library dependencies
dependencies = ['libprintf']

# @brief Custom Makefile
makefile = None

# @brief Requires POS
requires_pos = None
