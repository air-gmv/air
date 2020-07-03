##
# @package air.libraries.libcov
# @author lasg
# @brief libtest configuration

__author__ = 'lasg'

import os
import utils
from air import *
import utils.file as file_tools

# @brief Name
name = "libcov"

# @brief Description
description = "Coverage support library"

# @brief Library Source files
source_files = set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_LIBS_DIRECTORY, 'libcov'), ['.c']),
]))

# @brief Library Header files
header_files = set(utils.flatten([
        file_tools.getFilesByExtensions(
                os.path.join(SOURCE_LIBS_DIRECTORY, 'libcov', 'include'), ['.h']),
]))

# @brief Library dependencies
dependencies = ['libprintf']

# @brief Custom Makefile
makefile = None

# @brief Requires POS
requires_pos = None
