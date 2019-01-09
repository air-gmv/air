## @package parsers.a653.definitions
#  @author pfnf
#  @brief A653 XML definitions and parser utilities

import utils.parser as parserutils

ARINC_653_MODULE                            = 'ARINC_653_Module'

SYSTEM_HM									= 'System_HM_Table'
MODULE_HM									= "Module_HM_Table"
PARTITION_HM								= "Partition_HM_Table"

MODULE_LEVEL								= 'MODULE'
PARTITION_LEVEL								= 'PARTITION'
PROCESS_LEVEL								= 'PROCESS'

HM_STATE_ENTRY								= "System_State_Entry"
HM_STATE_ENTRY_ID							= "SystemState"
HM_STATE_ENTRY_DESCRIPTION					= "Description"

HM_ERROR_LEVEL								= "Error_ID_Level"
HM_ERROR_LEVEL_ID          					= 'ErrorIdentifier'
HM_ERROR_LEVEL_DESCRIPTION 					= 'Description'
HM_ERROR_LEVEL_TYPE       					= 'ErrorLevel'
HM_ERROR_LEVEL_CODE        					= 'ErrorCode'

HM_ERROR_ACTION								= "Error_ID_Action"
HM_ERROR_ACTION_ID          				= 'ErrorIdentifier'
HM_ERROR_ACTION_DESCRIPTION 				= 'Description'
HM_ERROR_ACTION_TYPE       					= 'Action'

PARTITION									= 'Partition'
PARTITION_ID      							= 'PartitionIdentifier'
PARTITION_NAME    							= 'PartitionName'
PARTITION_IS_SYSTEM     					= 'SystemPartition'
PARTITION_CRITICALITY   					= 'Criticality'
PARTITION_ENTRY_POINT  						= 'EntryPoint'
PARTITION_CALLBACK                          = 'PartitionCallback'

QUEUING_PORT                                = 'Queuing_Port'
QUEUING_PORT_NAME                           = 'Name'
QUEUING_PORT_MAX_MSG_SIZE                   = 'MaxMessageSize'
QUEUING_PORT_DIRECTION                      = 'Direction'
QUEUING_PORT_MAX_NB_MSGS                    = 'MaxNbMessages'
QUEUING_PORT_DISCIPLINE                     = 'Discipline'

SAMPLING_PORT                               = 'Sampling_Port'
SAMPLING_PORT_NAME                          = 'Name'
SAMPLING_PORT_MAX_MSG_SIZE                  = 'MaxMessageSize'
SAMPLING_PORT_DIRECTION                     = 'Direction'
SAMPLING_PORT_REFRESH_RATE                  = 'RefreshRateSeconds'

SCHEDULE 									= 'Module_Schedule'
SCHEDULE_ID       							= 'ScheduleIdentifier'
SCHEDULE_NAME     							= 'ScheduleName'
SCHEDULE_IS_INITIAL 						= 'InitialModuleSchedule'
SCHEDULE_MTF								= 'MajorFrameSeconds'

PARTITION_SCHEDULE							= 'Partition_Schedule'
PARTITION_SCHEDULE_PERIOD					= 'PeriodSeconds'
PARTITION_SCHEDULE_DURATION					= 'PeriodDurationSeconds'
PARTITION_SCHEDULE_SET_SCHEDULE				= 'SetModuleSchedule'
PARTITION_SCHEDULE_CHANGE_ACTION			= 'ScheduleChangeAction'

WINDOW										= 'Window_Schedule'
WINDOW_ID         							= 'WindowIdentifier'
WINDOW_START  								= 'WindowStartSeconds'
WINDOW_DURATION								= 'WindowDurationSeconds'
WINDOW_PERIOD_START   						= 'PartitionPeriodStart'

CONNECTION_TABLE                            = 'Connection_Table'
CHANNEL                                     = 'Channel'
CHANNEL_ID                                  = 'ChannelIdentifier'
CHANNEL_NAME                                = 'ChannelName'
CHANNEL_SRC                                 = 'Source'
CHANNEL_DST                                 = 'Destination'

CHANNEL_STANDARD_PARTITION                  = 'Standard_Partition'
CHANNEL_PORT_NAME                           = 'PortName'

PORT_SOURCE                                 = 'SOURCE'
PORT_DESTINATION                            = 'DESTINATION'

QUEUING_CHANNEL                             = 'QUEUING_PORT'
SAMPLING_CHANNEL                            = 'SAMPLING_PORT'

A653_TIMESCALE								= 1E9


VALID_NAME_TYPE				= [ str, lambda x : 0 < len(x) < 319 ]
VALID_FLOAT_TYPE			= [ parserutils.str2float, lambda x : x >= 0 ]
VALID_DECIMAL_TYPE			= [ parserutils.str2int, lambda x : x >= 0 ]
VALID_IDENTIFIER_TYPE		= [ parserutils.str2int, lambda x : x >= 0 ]
VALID_BOOLEAN_TYPE			= [ parserutils.str2bool, lambda x : isinstance(x, bool) ]
VALID_CRITICALITY_TYPE 		= [ str, lambda x : x in ['LEVEL_A', 'LEVEL_B', 'LEVEL_C', 'LEVEL_D', 'LEVEL_E'] ]
VALID_DIRECTION_TYPE 		= [ str, lambda x : x in [PORT_SOURCE, PORT_DESTINATION] ]
VALID_ERROR_LEVEL_TYPE		= [ str, lambda x : x in [MODULE_LEVEL, PARTITION_LEVEL, PROCESS_LEVEL] ]
VALID_ERROR_CODE_TYPE		= [ str, lambda x : x in ['DEADLINE_MISSED', 'APPLICATION_ERROR', 'NUMERIC_ERROR',
                                                      'ILLEGAL_REQUEST', 'STACK_OVERFLOW', 'MEMORY_VIOLATION',
                                                      'HARDWARE_FAULT', 'POWER_FAILURE'] ]
VALID_MODULE_ACTION_TYPE	= [ str, lambda x : x in ['IGNORE', 'SHUTDOWN', 'RESET'] ]
VALID_PARTITION_ACTION_TYPE	= [ str, lambda x : x in ['IGNORE', 'IDLE', 'WARM_START', 'COLD_START'] ]

SYSTEM_STATE_STR			= 'System State Entry (Id: {0:02X})'
ERROR_LEVEL_STR				= 'Error Level (Id: {0:02X}, Level: {1:^13})'
ERROR_ACTION_STR			= 'Error Action (Id: {0:02X}, Action: {1:^12})'
SCHEDULE_PARTITION_STR		= 'Partition Schedule (Id: {0}, Name: {1})'
PARTITION_STR				= 'Partition (Id: {0}, Name: {1})'
QUEUING_PORT_STR            = 'Queuing Port (Name: {0})'
SAMPLING_PORT_STR           = 'Sampling Port (Name: {0})'
SCHEDULE_STR				= 'Schedule (Id: {0}, Name: {1})'
WINDOW_STR_0				= 'Window (Id: {0})'
WINDOW_STR_1				= 'Empty Window'
CHANNEL_STR                 = 'Channel (Id: {0}, Name: {1})'


# System state
class SystemState(object):

    def __init__(self):
        self.id				= -1
        self.description	= ""
        self.errors			= []
        self.xml_line		= 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return SYSTEM_STATE_STR.format(self.id)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return '{0} - Description: {1}'.format(self, self.description)

# Error Level
class ErrorLevel(object):

    def __init__(self):
        self.id				= -1
        self.code			= ''
        self.level			= MODULE_LEVEL
        self.description	= ''
        self.xml_line		= 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ERROR_LEVEL_STR.format(self.id, self.level)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return '{0} - Code: {1:^20}; Description: {2}'.format(self, self.code, self.description)

# Error action
class ErrorAction(object):

    def __init__(self):
        self.id				= -1
        self.code			= ''
        self.action			= 'IGNORE'
        self.description	= ''
        self.xml_line		= 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ERROR_ACTION_STR.format(self.id, self.action)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return '{0} - Code: {1:^20}; Description: {2}'.format(self, self.code, self.description)

## Partition
class Partition(object):

    def __init__(self):
        self.index          = 0
        self.id				= -1
        self.name			= ''
        self.criticality	= 'LEVEL_E'
        self.is_system		= False
        self.entry_point	= ''
        self.hm_table		= []
        self.hm_callback    = ''
        self.directory      = ''
        self.executable     = ''
        self.ports          = []
        self.xml_line		= 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.id == other.id or (not self.name and self.name == other.name)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return PARTITION_STR.format(self.id, self.name)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return '{0} - Criticality: {1}; Is System: {2}; Entry Point: {3}' \
            .format(self, self.criticality, self.is_system, self.entry_point)

## Queuing Port
class QueuingPort(object):

    def __init__(self):
        self.index              = 0
        self.id                 = 0     # auto assigned during parsing
        self.name               = ''
        self.direction          = ''
        self.max_message_size   = 0
        self.max_nb_message     = 0
        self.discipline         = 0
        self.used               = False
        self.xml_line           = 0

    def __eq__(self, other):
        return self.name == other.name  # comparison only by name as the list contains queuing and sampling ports

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return QUEUING_PORT_STR.format(self.name)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return '{0} - Direction: {1}; Max Message Size: {2}; Max Number of Messages Point: {3}' \
            .format(self, self.direction, self.max_message_size, self.max_nb_message)

## Sampling Port
class SamplingPort(object):

    def __init__(self):
        self.index              = 0
        self.id                 = 0     # auto assigned during parsing
        self.name               = ''
        self.direction          = ''
        self.refresh_period     = 0
        self.max_message_size   = 0
        self.used               = False
        self.xml_line           = 0

    def __eq__(self, other):
        return self.name == other.name  # comparison only by name as the list contains queuing and sampling ports

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return SAMPLING_PORT_STR.format(self.name)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return '{0} - Direction: {1}; Max Message Size: {2}; Refresh Period: {3}' \
            .format(self, self.direction, self.max_message_size, self.refresh_period)

## Schedule
class Schedule(object):

    def __init__(self):
        self.index      = 0
        self.id 		= -1
        self.name 		= ''
        self.initial 	= False
        self.mtf 		= 0
        self.partitions	= []
        self.extensions	= None
        self.xml_line	= 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.id == other.id or (not self.name and self.name == other.name)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return SCHEDULE_STR.format(self.id, self.name)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return '{0} - Initial: {1}; Major Time Frame: {2}' \
            .format(self, self.initial, self.mtf)

    def get_partition_schedule(self, partition):
        matches = [ p for p in self.partitions if p.partition == partition ]
        return matches[0] if len(matches) > 0 else None

## Partition Schedule
class PartitionSchedule(object):

    def __init__(self):
        self.period			= 0
        self.duration		= 0
        self.set_schedule	= False
        self.change_action	= 'IGNORE'
        self.partition     	= None
        self.windows		= []
        self.xml_line		= 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and self.partition == other.partition

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return SCHEDULE_PARTITION_STR.format(self.partition.id, self.partition.name)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return '{0} - Period: {1}; Duration: {2}; Can set Schedule: {3}; Change Action: {4}' \
            .format(self, self.period, self.duration, self.set_schedule, self.change_action)

## Window
class Window(object):

    def __init__(self):
        self.id				= -1
        self.start			= 0
        self.duration		= 0
        self.period_start	= False
        self.partition		= None
        self.xml_line       = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return WINDOW_STR_0.format(self.id) if self.partition is not None else WINDOW_STR_1

    def __repr__(self):
        return self.__str__()

    def details(self):
        return '{0} - Start: {1}; Duration: {2}; Is Period Start: {3}' \
            .format(self, self.start / A653_TIMESCALE,
                    self.duration / A653_TIMESCALE, self.period_start)


## Channel
class Channel(object):

    def __init__(self):
        self.id         = 0
        self.name       = ''
        self.src        = []
        self.dst        = []
        self.type       = ''
        self.xml_line   = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.id == other.id or (not self.name and self.name == other.name)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return CHANNEL_STR.format(self.id, self.name)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()
