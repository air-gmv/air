## @package parser.xky_definitions
#  @author pfnf
#  @brief Contains the XKY extensions definitions

__author__ = 'pfnf'

import utils.parser as parserutils

MODULE_EXTENSION_OLD				= "air_module_ext"
MODULE_EXTENSION_TICKS_OLD			= "air_ticks_per_second"
MODULE_EXTENSION_CORES_OLD			= "air_cores"

MODULE_EXTENSION				    = "XKY_Configuration"
MODULE_EXTENSION_TICKS			    = "TicksPerSecond"
MODULE_EXTENSION_CORES			    = "RequiredCores"
MODULE_EXTENSION_MEMORY			    = "RequiredMemory"
MODULE_EXTENSION_TARGET             = "Target"

PARTITION_EXTENSION					= "PartitionConfiguration"
PARTITION_EXTENSION_CORES           = "Cores"
PARTITION_EXTENSION_PERSONALITY		= "Personality"
PARTITION_EXTENSION_IOP             = "IOP"
PARTITION_EXTENSION_LIBS            = "Libs"
PARTITION_EXTENSION_DEVICES         = "Devices"
PARTITION_EXTENSION_CACHE           = "Cache"
PARTITION_EXTENSION_CACHE_INST      = "CODE"
PARTITION_EXTENSION_CACHE_DATA      = "DATA"
PARTITION_EXTENSION_CACHE_NONE      = "NONE"
PARTITION_EXTENSION_MEMORY          = "Memory"
PARTITION_EXTENSION_MEMORY_SIZE     = "Size"
PARTITION_EXTENSION_MEMORY_UNIT     = "Unit"
PARTITION_EXTENSION_PERMISSIONS     = "Permissions"

WINDOW_EXTENSION					= "WindowConfiguration"
WINDOW_EXTENSION_ID					= "WindowIdentifier"
WINDOW_EXTENSION_CORES				= "Cores"

SHM_EXTENSION                       = "SharedMemory"
SHM_EXTENSION_ID                    = "Id"
SHM_EXTENSION_NAME                  = "Name"
SHM_EXTENSION_SIZE                  = "Size"
SHM_EXTENSION_UNIT                  = "Unit"

SHM_EXTENSION_PARTITION             = "PartitionAccess"
SHM_EXTENSION_PARTITION_PERMISSIONS = "Permissions"

SHM_EXTENSION_STR                   = "Shared Memory Area (Id: {0}, Name: {1})"
SHM_EXTENSION_PARTITION_STR         = "{0} Access Type : {1}"


XKY_PERMISSION_FPU_CONTROL			= 'FPU_CONTROL'
XKY_PERMISSION_CACHE_CONTROL		= 'CACHE_CONTROL'
XKY_PERMISSION_GLOBAL_TIME			= 'GLOBAL_TIME'
XKY_PERMISSION_SET_TOD				= 'SET_TOD'
XKY_PERMISSION_MODULE_CONTROL       = 'MODULE_CONTROL'

VALID_XKY_FILE      				= [ str, lambda x : len(x) > 0 ]
VALID_XKY_CACHE                     = [ parserutils.str2strList,
                                        lambda x: all(c.upper() in
                                                      [
                                                          PARTITION_EXTENSION_CACHE_INST,
                                                          PARTITION_EXTENSION_CACHE_DATA,
                                                          PARTITION_EXTENSION_CACHE_NONE
                                                      ] for c in x)]
## HM System States
HMSystemState = \
[
    dict(code='MODULE_INIT', description='module initialization'),
    dict(code='MODULE_EXEC', description='module execution'),
    dict(code='PARTITION_INIT', description='partition initialization'),
    dict(code='PARTITION_EXEC', description='partition execution'),
    dict(code='PARTITION_PROC', description='partition task/process'),
    dict(code='PARTITION_HM ', description='partition health-monitor'),
    dict(code='MODULE_HM ', description='module health-monitor'),
]

## HM Errors
HMErrorsDefinitions = \
[
    dict(code='POWER_ERROR', description='power error'),
    dict(code='VIOLATION_ERROR', description='violation error'),
    dict(code='SEGMENTATION_ERROR', description='segmentation error'),
    dict(code='UNIMPLEMENTED_ERROR', description='unimplemented error'),
    dict(code='FLOAT_ERROR', description='floating point error'),
    dict(code='ARITHMETIC_ERROR', description='arithmetic error'),
    dict(code='DIVISION_BY_0_ERROR', description='division by zero error'),
    dict(code='IO_ERROR', description='I/O error'),
    dict(code='USR0_ERROR', description='user defined error 0'),
    dict(code='USR1_ERROR', description='user defined error 1'),
    dict(code='USR2_ERROR', description='user defined error 2'),
    dict(code='USR3_ERROR', description='user defined error 3'),
    dict(code='USR4_ERROR', description='user defined error 4'),
    dict(code='USR5_ERROR', description='user defined error 5'),
    dict(code='USR6_ERROR', description='user defined error 6'),
    dict(code='USR7_ERROR', description='user defined error 7'),
    dict(code='USR4_ERROR', description='user defined error 8'),
    dict(code='USR5_ERROR', description='user defined error 9'),
    dict(code='USR6_ERROR', description='user defined error 10'),
    dict(code='USR7_ERROR', description='user defined error 11'),
    dict(code='USR4_ERROR', description='user defined error 12'),
    dict(code='USR5_ERROR', description='user defined error 13'),
    dict(code='USR6_ERROR', description='user defined error 14'),
    dict(code='USR7_ERROR', description='user defined error 15'),
]

## Shared Memory
class SharedMemory(object):

    def __init__(self):
        self.id         = -1
        self.name       = ''
        self.addr       = 0
        self.size       = 0
        self.unit       = 0
        self.permissions = []

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.id == other.id or (not self.name and self.name == other.name)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return SHM_EXTENSION_STR.format(self.id, self.name)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return '{0} - Size: {1}; Memory Unit: {2}' \
            .format(self, hex(self.size), hex(self.unit))

## Shared memory permissions
class SharedMemoryPermissions(object):

    def __init__(self):
        self.partition      = None
        self.permissions    = ""

    def __eq__(self, other):
        return isinstance(other, self.__class__) and self.partition == other.partition

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return SHM_EXTENSION_PARTITION_STR.format(self.partition, self.permissions)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()

## Align memory address
# @param addr memory address
# @param align alignment
# @return aligned memory address
def alignAddress(addr, align):

    return (addr + (align - 1)) & ~(align - 1)
