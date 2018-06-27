
SPW = 'SPW'
MIL = 'MIL'
ETH = 'ETH'
RTR = 'SPWRTR'
CAN = 'CAN'

SUPPORTED_DEVICES 	=   {	SPW : ['GRSPW'],
                        	MIL : ['GRMIL', 'BRM1553'],
                        	ETH : ['GRETH'],
                            RTR : ['SPWRTR'],
                            CAN : ['GRCAN']   }

IOP_PORT_TYPE		= { 'SamplingPort' : 'IOP_SAMPLING_PORT',
						'QueuingPort'  : 'IOP_QUEUING_PORT'   }



LOGICAL_DEVICE					= 'LogicalDevices/Device'
LOGICAL_DEVICE_ID				= 'Id'
LOGICAL_DEVICE_NAME				= 'Name'


REMOTE_PORT                     = 'RemotePorts/Port'
REMOTE_PORT_NAME                = 'Name'
REMOTE_PORT_LOGICAL_DEVICE_ID   = 'LogicalDeviceId'


PHYSICAL_DEVICE                 = 'PhysicalDevices/Device'
PHYSICAL_DEVICE_ID				= 'Id'
PHYSICAL_DEVICE_NAME			= 'Device'
PHYSICAL_DEVICE_PATTERN         = '(?P<type>[a-zA-Z]{1,})(?P<minor>[0-9]{1})'

ROUTE_PHYSICAL                  = 'PhysicalRoute'
ROUTE_LOGICAL                   = 'LogicalRoute'
ROUTE_ID                        = 'Id'

ROUTE_PHYSICAL_PORT             = 'PortId'
ROUTE_LOGICAL_DEVICE            = 'LogicalDeviceId'

#MIL LIST
MILLIST                         = 'MILList'
MIL_ID                          = 'Id'
MILLISTMJFRAME                  = 'MajorFrame'
#MIL SLOT
MILSLOT                         = 'Slot'
MILSLOTBUS                      = 'Bus'
MILSLOTTYPE                     = 'Type'
MILSLOTWCMODE                   = 'WCMode'
MILSLOTTIME                     = 'Time'

# Top Level Nodes
IOPARTITION						= 'IOPartition'


SCHEDULING                      = 'ModuleSchedules/Schedule'
SCHEDULING_ROUTE                = 'RoutesConfiguration/Route'
SCHEDULING_ROUTE_ID             = 'RouteId'
SCHEDULING_ROUTE_ACTIVE         = 'Active'
SCHEDULING_DEVICE               = 'DevicesConfiguration/Device'
SCHEDULING_DEVICE_ID            = 'DeviceId'

#Top level attributes
DATE_ATTR						= 'date'
NAME_ATTR						= 'name'
REQUEST_NUMBER_ATTR				= 'request_number'
TIME_TO_LIVE_ATTR				= 'time_to_live'
VERSION_ATTR					= 'version'

# ETH Header
ETHHEADER						= 'EthHeader'
ETHHEADER_IP					= 'Ip'
ETHHEADER_MAC					= 'MAC'
ETHHEADER_PORT					= 'Port'

#SPW Header
SPWHEADER                       = 'SpwHeader'
SPWHEADER_ADDRESS               = 'Address'

#CANBUS Header
CANHEADER                       = 'CanHeader'
CANHEADER_EXTENDED              = 'Extended'
CANHEADER_SSHOT					= 'Sshot'
CANHEADER_RTR                   = 'RTR'
CANHEADER_ID                    = 'CanID'

#MIL Header
MILHEADER                       = 'MILHeader'
MIL_ADDR                        = 'Addr'
MIL_SUBADDR                     = 'SubAddr'


import utils.parser as parserutils

VALID_STR				= [ str, lambda  x : len(x) ]
VALID_NAME_TYPE			= [ str, lambda x : 0 < len(x) < 31 ]
VALID_DEVICE_ID_TYPE	= [ parserutils.str2int, lambda x : x >= 0 ]
VALID_PORT			    = [ parserutils.str2int, lambda x : x > 0]
VALID_IP			    = [ lambda x: str(x).strip().split('.'), lambda x: len(x) == 4 ]
VALID_MAC			    = [ lambda x: str(x).strip().split(':'), lambda x: len(x) == 6 ]
VALID_SPW_ADDRESS       = [ lambda x: str(x).strip().split(','), lambda x: len(x) <= 32 ]
VALID_FLOAT_TYPE		= [ parserutils.str2float, lambda x : x >= 0 ]
VALID_DECIMAL_TYPE		= [ parserutils.str2int, lambda x : x >= 0 ]
VALID_BOOLEAN_TYPE		= [ parserutils.str2bool, lambda x : isinstance(x, bool) ]
VALID_DIRECTION_TYPE 	= [ str, lambda x : x in [ REMOTE_PORT_SRC, REMOTE_PORT_DST] ]
VALID_ID                = [ parserutils.str2int, lambda x: x > 0]
VALID_MASK_CODE         = [ lambda x: str(x).strip().split(':'), lambda x: len(x) == 4]
VALID_MILMJFRAME_TYPE   = [ parserutils.str2int, lambda x : 0 <= x <= 10000000 ]
VALID_MILADDR_TYPE      = [ parserutils.str2int, lambda x : 0 <= x <= 32 ]
VALID_MILSUBADDR_TYPE   = [ parserutils.str2int, lambda x : 0 <= x <= 32 ]
VALID_MILBUS_TYPE       = [ str, lambda x : x in [ 'A', 'B'] ]
VALID_MILTYPE           = [ str, lambda x : x in [ 'BC_RT', 'RT_BC', 'RT_RT'] ]
VALID_MILWCMODE_TYPE    = [ parserutils.str2int, lambda x : 0 <= x <= 1000000 ]
VALID_MILTIME_TYPE      = [ parserutils.str2int, lambda x : 0 <= x <= 1000000 ]



LOGICAL_DEVICE_STR      = 'Logical Device (Id: {0}, Name: {1})'
PHYSICAL_DEVICE_STR     = 'Physical Device (Id: {0}, Device: {1})'
ETH_HEADER_STR          = 'Ethernet Header (Mac: {0}, Ip: {1}, Port: {2})'
SPW_HEADER_STR          = 'SpaceWire Header (Address: {0})'
CAN_HEADER_STR          = 'Canbus Header (extended: {0}, Rtr: {1}, CanId: {2})'
MIL_HEADER_STR          = 'MIL-STD-1553 Header (ADDR: {0}, SUBADDR: {1})'
MIL_LIST_STR            = 'MIL LIST (Id: {0})'
MIL_SLOT_STR            = 'MIL SLOT (Id: {0} ADDR: {1}, SUBADDR: {2})'
REMOTE_PORT_STR         = 'Remote Port (Name: {0})'
ROUTE_STR               = 'Route (Id: {0})'
ROUTE_PHYSICAL_STR      = 'Physical Route (Id: {0})'
ROUTE_LOGICAL_STR       = 'Logical Route (Id: {0})'

## Logical Device
class LogicalDevice(object):

    def __init__(self):
        self.idx            = 0
        self.id             = 0
        self.name           = ''
        self.routes         = []

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.id == other.id or (not self.name and self.name == other.name)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return LOGICAL_DEVICE_STR.format(self.id, self.name)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()

## Physical Device
class PhysicalDevice(object):

    def __init__(self):
        self.idx            = 0
        self.id             = 0
        self.device         = ''
        self.type           = ''
        self.routes         = []

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.id == other.id or (not self.device and self.device == other.device)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return PHYSICAL_DEVICE_STR.format(self.id, self.device)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()

## Physical Route
class RoutePhysical(object):

    def __init__(self):
        self.id     = -1
        self.port   = None
        self.header = None

    def __eq__(self, other):
        return self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ROUTE_PHYSICAL_STR.format(self.id)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()

## Logical Route
class RouteLogical(object):

    def __init__(self):
        self.id     = -1
        self.device = None
        self.header = None

    def __eq__(self, other):
        return self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ROUTE_LOGICAL_STR.format(self.id)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()

## Route Schedule
class RouteSchedule(object):

    def __init__(self):
        self.index      = -1
        self.period     = 0
        self.duration   = 0
        self.routes     = []
        self.devices    = []

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.index == other.index

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ROUTE_PHYSICAL_STR.format(self.index)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()

## MIL List
class MILList(object):

    def __init__(self):
        self.id     = -1
        self.majorframe = 0
        self.slot       = []

    def __eq__(self, other):
        return self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return MIL_LIST_STR.format(self.id)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()

## MIL Slot
class MILSlot(object):

    def __init__(self):
        self.id     = -1
        self.bus   = 'A'
        self.type  = ''
        self.addr  = 0
        self.subaddr  = 0
        self.wcmode   = 0
        self.time  = 0

    def __eq__(self, other):
        return self.id == other.id

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return MIL_SLOT_STR.format(self.id, self.addr, self.subaddr)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()

## Ethernet Header
class EthHeader(object):

    def __init__(self):
        self.ip          = []
        self.mac         = []
        self.port        = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.ip == other.ip and self.mac == other.mac and self.port == other.port

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return ETH_HEADER_STR.format(':'.join(self.mac), '.'.join(self.ip), self.port)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()
        
## SpaceWire Header
class SpwHeader(object):

    def __init__(self):
        self.address     = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
               self.address == other.address

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return SPW_HEADER_STR.format(self.address)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()
    
## CANBUS Header
class CanHeader(object):
    def __init__(self):
        self.extended = False
        self.sshot = 0
        self.rtr = 0
        self.can_id = 0
        
    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
            self.can_id == other.can_id
            
    def __ne__(self, other):
        return not self.__eq__(other)
    
    def __str__(self):
        return CAN_HEADER_STR.format(self.extended, \
                self.sshot, self.rtr, self.can_id)
    
    def __repr__(self):
        return self.__str__()
    
    def details(self):
        return self.__str__()


## MIL-STD-1553 Header
class MILHeader(object):
    def __init__(self):
        self.desc = 0
        self.address = 0

    def __eq__(self, other):
        return isinstance(other, self.__class__) and \
            self.desc == other.desc and self.address == other.address

    def __ne__(self, other):
        return not self.__eq__(other)

    def __str__(self):
        return MIL_HEADER_STR.format(self.desc, \
                self.address)

    def __repr__(self):
        return self.__str__()

    def details(self):
        return self.__str__()
