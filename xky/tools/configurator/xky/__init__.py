## @package xky
#  @author pfnf
#  @brief XKY directory configuration

__author__ = 'pfnf'


from os import path, getcwd
from collections import namedtuple

LIBS_DIR_NAME                   = 'libs'
INSTALL_DIR_NAME                = 'install'
PMK_DIR_NAME                    = 'pmk'
POS_DIR_NAME                    = 'pos'
TOOLS_DIR_NAME                  = 'tools'
PAL_DIR_NAME                    = 'libxky'

ROOT_DIRECTORY                  = path.abspath(path.join(path.dirname(__file__), '..', '..', '..'))
SOURCE_LIBS_DIRECTORY           = path.abspath(path.join(ROOT_DIRECTORY, LIBS_DIR_NAME))
SOURCE_PAL_DIRECTORY            = path.abspath(path.join(SOURCE_LIBS_DIRECTORY, PAL_DIR_NAME))
SOURCE_PMK_DIRECTORY            = path.abspath(path.join(ROOT_DIRECTORY, PMK_DIR_NAME))
SOURCE_POS_DIRECTORY            = path.abspath(path.join(ROOT_DIRECTORY, POS_DIR_NAME))
SOURCE_TOOLS_DIRECTORY          = path.abspath(path.join(ROOT_DIRECTORY, TOOLS_DIR_NAME))

INSTALL_DIRECTORY               = path.abspath(path.join(ROOT_DIRECTORY, INSTALL_DIR_NAME))
INSTALL_LIBS_DIRECTORY          = path.abspath(path.join(INSTALL_DIRECTORY, LIBS_DIR_NAME))
INSTALL_PMK_DIRECTORY           = path.abspath(path.join(INSTALL_DIRECTORY, PMK_DIR_NAME))
INSTALL_POS_DIRECTORY           = path.abspath(path.join(INSTALL_DIRECTORY, POS_DIR_NAME))
INSTALL_TOOL_DIRECTORY          = path.abspath(path.join(INSTALL_DIRECTORY, TOOLS_DIR_NAME))


CONFIGURATOR_DIRECTORY          = path.abspath(path.join(path.dirname(__file__), '..'))
POS_DIRECTORY                   = path.abspath(path.join(CONFIGURATOR_DIRECTORY, 'xky', 'pos'))
LIBRARIES_DIRECTORY             = path.abspath(path.join(CONFIGURATOR_DIRECTORY, 'xky', 'libraries'))
TARGETS_DIRECTORY               = path.abspath(path.join(CONFIGURATOR_DIRECTORY, 'xky', 'targets'))
TEMPLATES_DIRECTORY             = path.abspath(path.join(CONFIGURATOR_DIRECTORY, 'templates'))
XKY_TEMPLATES_DIRECTORY         = path.abspath(path.join(TEMPLATES_DIRECTORY, 'xky_configuration'))
APP_TEMPLATES_DIRECTORY         = path.abspath(path.join(TEMPLATES_DIRECTORY, 'app_configuration'))
WORKING_DIRECTORY               = path.abspath(getcwd())

## POS and BSP configurations structures ##
POS = namedtuple('xkyPOS', ['libs', 'alias'])
LIB = namedtuple('xkyLib', ['name', 'depends'])
MMAP =namedtuple('xkyMMAP', ['kernel_space', 'partition_space', 'units', 'default_unit'])
IOP = namedtuple('xkyIOP', ['defines', 'devices', 'drivers', 'alias', 'arch'])

##
# @brief Health-Monitor definitions
MIN_STATE_ID = 0		              # match health_monitor.h definition
MAX_STATE_ID = 7		              # match health_monitor.h definition
MIN_ERROR_ID = 0		              # match health_monitor.h definition
MAX_ERROR_ID = 21	                  # match health_monitor.h definition

## Permissions definitions ##
PERMISSION_SUPERVISOR           = 'SUPERVISOR'
PERMISSION_FPU                  = 'FPU_CONTROL'
PERMISSION_CACHE                = 'CACHE_CONTROL'
PERMISSION_SET_TOD              = 'SET_TOD'
PERMISSION_SET_PARTITION        = 'SET_PARTITION_MODE'
PERMISSION_GLOBAL_TIME          = 'GLOBAL_TIME'
PERMISSION_MODULE               = 'MODULE_CONTROL'

