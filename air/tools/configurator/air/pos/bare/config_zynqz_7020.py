##
# @package air.pos.bare
# @author huca
# @brief Bare application executive configuration

__author__ = 'huca'

import os
import utils
from air import *
import utils.file as file_tools

# @brief Name
name = "bare"

# @brief Description
description = "Bare C support"

# @brief OS alias
alias = None

# @brief Supported git checkout id
git_id = ""

# @brief POS Source files
source_files = {
    'arm': set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_POS_DIRECTORY, 'bare', 'arm'), ['.c', '.S']),
        file_tools.getFilesByExtensions(os.path.join(SOURCE_LIBS_DIRECTORY, 'libprintf'), ['.c', '.S'])
    ])),
    'sparc': set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_POS_DIRECTORY, 'bare', 'sparc'), ['.c', '.S'])
    ])),
}

# @brief POS Private Header files
private_header_files = {
    'arm': set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_POS_DIRECTORY, 'bare', 'arm'), ['.h', '.ld']),
        file_tools.getFilesByExtensions(os.path.join(SOURCE_LIBS_DIRECTORY, 'libprintf'), ['.h', '.ld'])
    ])),
    'sparc': set(utils.flatten([
        file_tools.getFilesByExtensions(os.path.join(SOURCE_POS_DIRECTORY, 'bare', 'sparc'), ['.h', '.ld'])
    ])),
}

# @brief POS Public Header files
public_header_files = {
    'arm': [h for h in private_header_files['arm']
              if h.endswith('bare.h') or h.endswith('linkcmds.ld')],
    'sparc': [h for h in private_header_files['sparc']
              if h.endswith('bare.h') or h.endswith('linkcmds.ld')],
}

# @brief Custom Makefile
makefile = None

# @brief List of lines to be add to Makefile.inc
makefile_inc = [
    "ifeq ($(RVS_ENABLED),1)",
    "TARGET_CC=/usr/local/pkg/RVS/v3.14/integration_resources/compiler_wrappers/gcc/gcc_wrap",
    "TARGET_LD=/usr/local/pkg/RVS/v3.14/integration_resources/compiler_wrappers/gcc/ld_wrap",
    "endif",
]

# @brief List of application partition templates
partition_templates = [
    os.path.join(APP_TEMPLATES_DIRECTORY, 'pos_makefile.mako'),     # POS makefile
    os.path.join(POS_DIRECTORY, 'bare', 'init.c.mako'),
]
