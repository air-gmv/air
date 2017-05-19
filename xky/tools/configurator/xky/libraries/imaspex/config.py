##
# @package xky.libraries.libprintf
# @author pfnf
# @brief libprintf configuration

__author__ = 'pfnf'

import os
import utils
from xky import *
import utils.file as file_tools

# @brief Name
name = "imaspex"

# @brief Description
description = "IMASPEX support library"

# @brief Library Source files
source_files = set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_LIBS_DIRECTORY, 'imaspex'), ['.c']),
]))

# @brief Library Header files
header_files = set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_LIBS_DIRECTORY, 'imaspex'), ['.h']),
]))

# @brief Library dependencies
dependencies = []

# @brief Custom Makefile
makefile = None

# @brief Requires POS
requires_pos = None
